//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include "ContinuityModelRealization.h"

ContinuityModelRealization::ContinuityModelRealization(const vector<WaterSource *> &water_sources,
                                                       const Graph &water_sources_graph,
                                                       const vector<vector<int>> &water_sources_to_utilities,
                                                       const vector<Utility *> &utilities,
                                                       const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                                                       const unsigned int realization_id) :
        ContinuityModel(water_sources, utilities, water_sources_graph,
                        water_sources_to_utilities, realization_id),
        drought_mitigation_policies(drought_mitigation_policies) {

    /// Pass corresponding utilities to drought mitigation instruments.
    for (DroughtMitigationPolicy *dmp : this->drought_mitigation_policies) {
        dmp->addSystemComponents(utilities, water_sources);
    }
}

ContinuityModelRealization::ContinuityModelRealization(ContinuityModelRealization &continuity_model_realization)
        : ContinuityModel(continuity_model_realization.continuity_water_sources,
                          continuity_model_realization.continuity_utilities,
                          continuity_model_realization.water_sources_graph,
                          continuity_model_realization.water_sources_to_utilities,
                          continuity_model_realization.realization_id) {
}

ContinuityModelRealization::~ContinuityModelRealization() {

}

vector<WaterSource *> ContinuityModelRealization::getWater_sources() {
    return continuity_water_sources;
}

void ContinuityModelRealization::setShortTermROFs(const vector<double> &risks_of_failure) {
    for (unsigned long i = 0; i < continuity_utilities.size(); ++i) {
        continuity_utilities.at(i)->setRisk_of_failure(risks_of_failure.at(i));
    }
}

void ContinuityModelRealization::setLongTermROFs(const vector<double> &risks_of_failure, const int week) {
    for (unsigned long i = 0; i < continuity_utilities.size(); ++i) {
        continuity_utilities.at(i)->infrastructureConstructionHandler(risks_of_failure.at(i), week);
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
