//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include <algorithm>
#include "ContinuityModelRealization.h"

ContinuityModelRealization::ContinuityModelRealization(
        vector<WaterSource *> &water_sources,
        const Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvFlowControl *> &min_env_flow_control,
        vector<double>& utilities_rdm,
        vector<double>& water_sources_rdm,
        vector<double>& policy_rdm,
        const unsigned int realization_id)
        : ContinuityModel(water_sources, utilities, min_env_flow_control, water_sources_graph,
                          water_sources_to_utilities, utilities_rdm, water_sources_rdm,
                          realization_id),
          drought_mitigation_policies(drought_mitigation_policies) {

    // Pass corresponding utilities to drought mitigation instruments.
    for (DroughtMitigationPolicy *dmp : this->drought_mitigation_policies) {
        dmp->addSystemComponents(utilities, water_sources, min_env_flow_control);
        dmp->setRealization(realization_id, utilities_rdm, water_sources_rdm,
                            policy_rdm);
    }
}

ContinuityModelRealization::~ContinuityModelRealization() {
    // Delete drought mitigation policies.
    for (auto dmp : drought_mitigation_policies) {
        delete dmp;
    }
}

void ContinuityModelRealization::setShortTermROFs(const vector<vector<double>> &risks_of_failure) {
    int storage_row = 0;
    int treatment_row = 1;
    for (unsigned long i = 0; i < continuity_utilities.size(); ++i) {
        double high_rof_value = max(risks_of_failure[i][storage_row], risks_of_failure[i][treatment_row]);
        continuity_utilities.at(i)->setRisk_of_failure(high_rof_value);
        continuity_utilities.at(i)->setRisk_of_failures(
                risks_of_failure[i][storage_row],
                risks_of_failure[i][treatment_row]);
    }
}

void ContinuityModelRealization::setLongTermROFDemandProjectionEstimate(const vector<Utility *> &rof_utilities) {
    for (unsigned long i = 0; i < continuity_utilities.size(); ++i) {
        continuity_utilities.at(i)->setCurrent_year_demand_record(rof_utilities.at(i)->getCurrent_year_demand_record());
        continuity_utilities.at(i)->setFuture_demand_estimate(rof_utilities.at(i)->getFuture_demand_estimate());
    }
}

void ContinuityModelRealization::updateJointWTPTreatmentAllocations(int current_week) {
    // TODO: After year 0 of the joint WTP under variable allocations, when allocations are set based on
    // how much treatment might be used by each utility based on the storage they have in Jordan Lake
    // and should be determined based on a Utility::splitDemands-esque routine, then allocations are increased
    // based on the expected growth by each partner until capacity in the plant is maxed
    int year = round(current_week/WEEKS_IN_YEAR_ROUND);
    for (WaterSource *ws : continuity_water_sources)
        if (ws->isOnline())
            if (ws->source_type == NEW_JOINT_WATER_TREATMENT_PLANT)
                if (ws->getAgreementType() == NEW_JOINT_WATER_TREATMENT_PLANT_VARIABLE_ALLOCATIONS) {
                    vector<double> demand_deltas;
                    for (int u : *continuity_water_sources.at(ws->getParentWaterSourceID())->getUtilities_with_allocations())
                        if (u != continuity_water_sources.at(ws->getParentWaterSourceID())->getWaterQualityPoolID())
                            demand_deltas.push_back(continuity_utilities.at(u)->calculateCurrentToNextYearDemandDelta(year));

                    // reset treatment capacity allocations based on demand ratios
                    // and then adjust levels in parent source to match
                    // NOTE: allocated treatment capacity vector for the parent reservoir needs to line up with
                    // the elements of the one from the JointWTP or allocations will be adjusted incorrectly
                    // (this should be impossible to do when the objects are initialized, but you never know...)
                    vector<double> previous_year_treatment_capacities = ws->getAllocatedTreatmentCapacities();
                    ws->resetAllocations(&demand_deltas); // should skip right to VariableJointWTP definition
                    continuity_water_sources.at(ws->getParentWaterSourceID())->resetTreatmentAllocations(
                            previous_year_treatment_capacities,
                            ws->getAllocatedTreatmentCapacities());

                    // be sure to also adjust utility treatment capacity as capacity changes for the reservoirs
                    for (int u : *ws->getUtilities_with_allocations())
                        continuity_utilities.at(u)->updateTreatmentCapacity(
                                ws->getAllocatedTreatmentCapacities()[u] - previous_year_treatment_capacities[u]);
                }
}

void ContinuityModelRealization::setLongTermROFs(const vector<vector<double>> &risks_of_failure, const int week) {
    vector<int> new_infra_triggered;
    int nit; // new infrastruction triggered - id.
    int storage_row = 0;
    int treatment_row = 1;

    // Loop over utilities to see if any of them will build new infrastructure.
    for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
        /// OCT 2019: TRIAGE ROFs; FROM EITHER STORAGE OR TREATMENT ROF, CHOOSE THE GREATEST
        /// VALUE TO PASS TO DECIDE WHETHER TO EXPAND INFRASTRUCTURE (0: storage ROF row, 1: treatment)
        double high_rof_value = max(risks_of_failure[u][storage_row], risks_of_failure[u][treatment_row]);
        continuity_utilities[u]->setLongTermRisk_of_failures(
                risks_of_failure[u][storage_row],
                risks_of_failure[u][treatment_row]);

        // Runs utility's infrastructure construction handler and get the id
        // of new source built, if any.
        nit = continuity_utilities[u]->infrastructureConstructionHandler(high_rof_value, week);
        // If new source was built, check add it to the list of sources
        // built by all utilities.
        if (nit != NON_INITIALIZED)
            new_infra_triggered.push_back(nit);
    }

    // Look for and remove duplicates, in the unlikely case two utilities
    // build the same source at the same time. This will prevent the source
    // from being erased from a utility which will later try to build it.
    sort(new_infra_triggered.begin(),
         new_infra_triggered.end());
    new_infra_triggered.erase(unique(new_infra_triggered.begin(),
                                     new_infra_triggered.end()),
                              new_infra_triggered.end());

    // If infrastructure was built, force utilities to build their share of
    // that infrastructure option (which will only happen it the listed
    // option is in the list of sources to be built for other utilities.
    if (!new_infra_triggered.empty())
        for (Utility *u : continuity_utilities) {
            u->forceInfrastructureConstruction(week, new_infra_triggered);
        }
}

void ContinuityModelRealization::applyDroughtMitigationPolicies(int week) {
    for (DroughtMitigationPolicy* dmp : drought_mitigation_policies) {
        //cout << dmp->id << ", " << dmp->type << endl;
        dmp->applyPolicy(week);
    }
}

const vector<DroughtMitigationPolicy *> ContinuityModelRealization::getDrought_mitigation_policies() const {
    return drought_mitigation_policies;
}
