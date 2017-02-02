//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include "ContinuityModelRealization.h"

ContinuityModelRealization::ContinuityModelRealization(const vector<WaterSource *> &water_source,
                                                       const vector<vector<int>> &water_source_connectivity_matrix,
                                                       const vector<Utility *> &utilities,
                                                       const vector<vector<int>> &water_source_utility_connectivity_matrix,
                                                       const int realization_index) :
        ContinuityModel(water_source, water_source_connectivity_matrix, utilities,
                        water_source_utility_connectivity_matrix),
        realization_id(realization_index) {}


vector<WaterSource *> ContinuityModelRealization::getWater_sources() {
    return water_sources;
}

void ContinuityModelRealization::setRisks_of_failure(const vector<double> &risks_of_failure) {
    ContinuityModelRealization::risks_of_failure = risks_of_failure;
}

const vector<double> &ContinuityModelRealization::getRisks_of_failure() const {
    return risks_of_failure;
}
