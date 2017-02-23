//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include "ContinuityModelRealization.h"

ContinuityModelRealization::ContinuityModelRealization(const vector<WaterSource *> &water_source,
                                                       const Graph &water_sources_graph,
                                                       const vector<vector<int>> &water_sources_to_utilities,
                                                       const vector<Utility *> &utilities,
                                                       const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                                                       const int realization_index) :
        ContinuityModel(water_source, utilities, water_sources_graph,
                        water_sources_to_utilities),
        realization_id(realization_index), drought_mitigation_policies(drought_mitigation_policies) {

    /// Pass corresponding utilities to drought mitigation instruments.
    for (DroughtMitigationPolicy *dmp : this->drought_mitigation_policies) {
        for (int i : dmp->getUtilities_ids()) {
            dmp->addUtility(this->utilities.at((unsigned long) i));
        }
    }
}

vector<WaterSource *> ContinuityModelRealization::getWater_sources() {
    return water_sources;
}

void ContinuityModelRealization::setRisks_of_failure(const vector<double> &risks_of_failure) {
    for (unsigned long i = 0; i < utilities.size(); ++i) {
        utilities.at(i)->setRisk_of_failure(risks_of_failure.at(i));
    }
}

void ContinuityModelRealization::applyDroughtMitigationPolicies(int week) {
    for (unsigned long i = 0; i < drought_mitigation_policies.size(); ++i) {
        drought_mitigation_policies.at(i)->applyPolicy(week);
    }
}

const vector<DroughtMitigationPolicy *> ContinuityModelRealization::getDrought_mitigation_policies() const {
    return drought_mitigation_policies;
}
