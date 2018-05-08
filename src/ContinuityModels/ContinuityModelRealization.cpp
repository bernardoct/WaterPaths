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

    /// Pass corresponding utilities to drought mitigation instruments.
    for (DroughtMitigationPolicy *dmp : this->drought_mitigation_policies) {
        dmp->addSystemComponents(utilities, water_sources, min_env_flow_control);
        dmp->setRealization(realization_id, utilities_rdm, water_sources_rdm,
                            policy_rdm);
    }
}

ContinuityModelRealization::~ContinuityModelRealization() {
    /// Delete drought mitigation policies.
    for (auto dmp : drought_mitigation_policies) {
        delete dmp;
    }
}

void ContinuityModelRealization::setShortTermROFs(const vector<double> &risks_of_failure) {
    for (unsigned long i = 0; i < continuity_utilities.size(); ++i) {
        continuity_utilities.at(i)->setRisk_of_failure(risks_of_failure.at(i));
    }
}

void ContinuityModelRealization::setLongTermROFs(const vector<double> &risks_of_failure, const int week) {
    vector<int> new_infra_triggered;
    int nit; // new infrastruction triggered - id.


    /// At the beginning of each year, update allocations in shared reservoirs as desired
    vector<vector<double>> temp_allocations(continuity_water_sources.size(),
                                            vector<double>(continuity_utilities.size()));
    vector<int> temp_parent_source_ids;
    vector<int> temp_actual_source_ids;
    for (unsigned long ws = 0; ws < continuity_water_sources.size(); ++ws) {
        continuity_water_sources[ws]->updateTreatmentAndCapacityAllocations(week);

        /// Hold treatment allocation capacity from last year as they are updated
        /// And pass to JointWTP the parent reservoir treatment capacity for calculating
        /// demand fractions
        for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
            temp_allocations[ws][u] = continuity_water_sources[ws]->getAllocatedTreatmentCapacity(u);
        }

        if (continuity_water_sources[ws]->getUtilities_with_allocations() != NULL
            && continuity_water_sources[ws]->getParentSourceID() != NON_INITIALIZED)
            for (int u : *continuity_water_sources[ws]->getUtilities_with_allocations())
                continuity_water_sources[ws]->
                        recordParentReservoirTreatmentCapacity(u,
                                                               continuity_water_sources[continuity_water_sources[ws]->
                                                                       getParentSourceID()]->
                                                                       getAllocatedTreatmentCapacity(u));

        continuity_water_sources[ws]->updateTreatmentAllocations(week, realization_demands);

        /// Check if each water source has its allocations changed, and if so record the parent source id
        for (unsigned long u = 0; u < continuity_utilities.size(); ++u)
            if (temp_allocations[ws][u] != continuity_water_sources[ws]->getAllocatedTreatmentCapacity(u)) {
                temp_parent_source_ids.push_back(continuity_water_sources[ws]->getParentSourceID());
                temp_actual_source_ids.push_back(continuity_water_sources[ws]->id);
            }
    }

    /// Remove repeated ids from temporary source id vectors created above
    sort(temp_parent_source_ids.begin(),
         temp_parent_source_ids.end());
    temp_parent_source_ids.erase(unique(temp_parent_source_ids.begin(),
                                        temp_parent_source_ids.end()),
                                 temp_parent_source_ids.end());

    sort(temp_actual_source_ids.begin(),
         temp_actual_source_ids.end());
    temp_actual_source_ids.erase(unique(temp_actual_source_ids.begin(),
                                        temp_actual_source_ids.end()),
                                 temp_actual_source_ids.end());


    /// Do treatment allocations at parent water sources need to be updated?
    for (int ws = 0; ws < temp_actual_source_ids.size(); ws++) {
        if (temp_actual_source_ids.size() != temp_parent_source_ids.size()) {
            cout << "Error in treatment allocation adjustment: parent ID and source ID vectors must be equal length." << endl;
            cout << "For now, assuming this means the same parent reservoir is tied to two different source ids." << endl;
            for (int i = temp_parent_source_ids.size(); i < temp_actual_source_ids.size(); i++)
                temp_parent_source_ids.push_back(temp_parent_source_ids[i-1]);
        }

        if (temp_parent_source_ids[ws] == NON_INITIALIZED) {} else {
            for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
                /// Add capacity to each utility treatment allocation based on observed change

                cout << "Utility " << u << " treatment allocation fraction (before):" << endl;
                cout << continuity_water_sources[temp_parent_source_ids[ws]]->getAllocatedTreatmentFraction(u) << endl;

                double allocation_adjustment =
                        continuity_water_sources[temp_actual_source_ids[ws]]->getAllocatedTreatmentCapacity(u)
                        - temp_allocations[temp_actual_source_ids[ws]][u];

                continuity_water_sources[temp_parent_source_ids[ws]]->
                        addAllocatedTreatmentCapacity(allocation_adjustment, u);

                cout << "Utility " << u << " treatment allocation fraction (after):" << endl;
                cout << continuity_water_sources[temp_parent_source_ids[ws]]->getAllocatedTreatmentFraction(u) << endl;

                /// Similarily, add capacity to each utility capacity allocation
                /// The previous statements re-allocate the parent source treatment allocations
                /// Adjust the storage capacity allocations to be proportionately equal
                continuity_water_sources[temp_parent_source_ids[ws]]
                        ->setAllocatedSupplyCapacity(
                                continuity_water_sources[temp_parent_source_ids[ws]]
                                        ->getAllocatedTreatmentFraction(u),
                                u);

                /// Record allocation adjustment to determine retroactive debt service payments
                continuity_water_sources[temp_actual_source_ids[ws]]->setAllocationAdjustment(u, allocation_adjustment);
            }
            /// Make sure that these allocations do not disrupt water balance, entire capacity of
            /// parent reservoir must be accounted for
            continuity_water_sources[temp_parent_source_ids[ws]]->normalizeAllocatedSupplyCapacity();

        }
    }


    /// Loop over utilities to see if any of them will build new infrastructure.
    for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
        /// Runs utility's infrastructure construction handler and get the id
        /// of new source built, if any.
        nit = continuity_utilities[u]->
                infrastructureConstructionHandler(risks_of_failure[u], week);
        /// If new source was built, check add it to the list of sources
        /// built by all utilities.
        if (nit != NON_INITIALIZED)
            new_infra_triggered.push_back(nit);
    }

    /// Look for and remove duplicates, in the unlikely case two utilities
    /// build the same source at the same time. This will prevent the source
    /// from being erased from a utility which will later try to build it.
    sort(new_infra_triggered.begin(),
         new_infra_triggered.end());
    new_infra_triggered.erase(unique(new_infra_triggered.begin(),
                                     new_infra_triggered.end()),
                              new_infra_triggered.end());

    /// Issue bonds for triggered project to triggering utility
    /// as well as any that are partners
    for (int it : new_infra_triggered) {
        for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
            auto temp_infra_options = continuity_utilities[u]->getRof_infrastructure_construction_order();
            auto temp_infra_options_demand = continuity_utilities[u]->getDemand_infra_construction_order();

            temp_infra_options.insert(temp_infra_options.end(),
                                      temp_infra_options_demand.begin(), temp_infra_options_demand.end());
            /// If a utility has the triggered project on its list of potential projects
            auto irange = find(temp_infra_options.begin(), temp_infra_options.end(), it);
            if (temp_infra_options.end() != irange)
                continuity_utilities[u]->infrastructureBondHandler(week, it);
        }
    }

    /// If infrastructure was built, force utilities to build their share of
    /// that infrastructure option (which will only happen it the listed
    /// option is in the list of sources to be built for other utilities.
    if (!new_infra_triggered.empty())
        for (Utility *u : continuity_utilities) {
            u->forceInfrastructureConstruction(week,
                                               new_infra_triggered);
        }

}

void ContinuityModelRealization::applyDroughtMitigationPolicies(int week) {
    for (DroughtMitigationPolicy* dmp : drought_mitigation_policies) {

        dmp->applyPolicy(week);
    }
}

const vector<DroughtMitigationPolicy *> ContinuityModelRealization::getDrought_mitigation_policies() const {
    return drought_mitigation_policies;
}
