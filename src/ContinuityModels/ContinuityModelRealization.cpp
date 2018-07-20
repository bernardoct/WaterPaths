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
    //updateAllocations(week);

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

void ContinuityModelRealization::updateAllocations(const int week) {
    vector<vector<double>> temp_allocations(continuity_water_sources.size(),
                                            vector<double>(continuity_utilities.size()));

    vector<int> temp_actual_source_ids;
    for (unsigned long ws = 0; ws < continuity_water_sources.size(); ++ws) {
        continuity_water_sources[ws]->updateTreatmentAndCapacityAllocations(week);

        /// Hold treatment allocation capacity from last year as they are updated
        /// And pass to JointWTP the parent reservoir treatment capacity for calculating
        /// demand fractions
        for (int u = 0; u < continuity_utilities.size(); ++u)
            temp_allocations[ws][u] = continuity_water_sources[ws]->getAllocatedTreatmentCapacity(u);

        if (continuity_water_sources[ws]->getUtilities_with_allocations() != nullptr
            && continuity_water_sources[ws]->getParentSourceID() != NON_INITIALIZED)
            for (int u : *continuity_water_sources[ws]->getUtilities_with_allocations())
                continuity_water_sources[ws]->
                        recordParentReservoirTreatmentCapacity(u,
                                                               continuity_water_sources[continuity_water_sources[ws]->
                                                                       getParentSourceID()]->
                                                                       getAllocatedTreatmentCapacity(u));

        continuity_water_sources[ws]->updateTreatmentAllocations(week, realization_demands, realization_supply_demands);

        /// Check if each water source has its allocations changed, and if so record the parent source id
        for (int u = 0; u < continuity_utilities.size(); ++u)
            if (temp_allocations[ws][u] != continuity_water_sources[ws]->getAllocatedTreatmentCapacity(u))
                temp_actual_source_ids.push_back(continuity_water_sources[ws]->id);
    }

    /// Remove repeated ids from temporary source id vector created above
    sort(temp_actual_source_ids.begin(),
         temp_actual_source_ids.end());
    temp_actual_source_ids.erase(unique(temp_actual_source_ids.begin(),
                                        temp_actual_source_ids.end()),
                                 temp_actual_source_ids.end());

    /// If source above was flagged as having its treatment capacities changed,
    /// then if that source has a parent reservoir, its allocations must also change.
    int parent_source_id;
    for (int actual_ws_id : temp_actual_source_ids) {
        /// Determine parent source id
        parent_source_id = continuity_water_sources[actual_ws_id]->getParentSourceID();

        cout << "Treatment allocations are being adjusted for actual source " << actual_ws_id
             << " and its parent source " << parent_source_id << endl;

        if (parent_source_id == NON_INITIALIZED) {} else {
            for (int u = 0; u < continuity_utilities.size(); ++u) {
                /// Add capacity to each utility treatment allocation based on observed change

                cout << "Utility " << u << " parent source treatment allocation fraction (before):" << endl;
                cout << continuity_water_sources[parent_source_id]->getAllocatedTreatmentFraction(u) << endl;

                /// This quantity is the difference between the past period allocation for a utility
                /// and the quantity it was changed to in the above loop
                double allocation_adjustment =
                        continuity_water_sources[actual_ws_id]->getAllocatedTreatmentCapacity(u)
                        - temp_allocations[actual_ws_id][u];

                /// The difference in capacity is applied to allocations on the parent source
                /// This may result temporarily in mis-calculated allocation fractions, which are
                /// properly normalized below
                continuity_water_sources[parent_source_id]->addAllocatedTreatmentCapacity(allocation_adjustment, u);

                cout << "Utility " << u << " parent source treatment allocation fraction (after):" << endl;
                cout << continuity_water_sources[parent_source_id]->getAllocatedTreatmentFraction(u) << endl;

                /// Similarly, add capacity to each utility capacity allocation
                /// The previous statements re-allocate the parent source treatment allocations
                /// Adjust the storage capacity allocations to be proportionately equal
                /// UPDATE: CAPACITY ALLOCATIONS SHOULD NOT BE ADJUSTED, ONLY TREATMENT CAPACITY
//                continuity_water_sources[parent_source_id]->setAllocatedSupplyCapacity(
//                                continuity_water_sources[parent_source_id]->getAllocatedTreatmentFraction(u), u);

                /// Record allocation adjustment to determine retroactive debt service payments
                continuity_water_sources[actual_ws_id]->setAllocationAdjustment(u, allocation_adjustment);
            }
            /// Make sure that these allocations do not disrupt water balance, entire capacity of
            /// parent reservoir must be accounted for
            continuity_water_sources[parent_source_id]->normalizeAllocations();
        }
    }
}
