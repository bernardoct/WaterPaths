//
// Created by bct52 on 6/28/17.
//

#include "MinEnvironFlowControl.h"

MinEnvironFlowControl::MinEnvironFlowControl(const vector<int> &water_sources_ids, const vector<int> &catchments_ids,
                                             const vector<int> &utilities_ids) : water_sources_ids(water_sources_ids),
                                                                                 catchments_ids(catchments_ids),
                                                                                 utilities_ids(utilities_ids) {}

void MinEnvironFlowControl::addComponents(vector<WaterSource *> water_sources, vector<Utility *> utilities,
                                          vector<Catchment *> catchments) {
    this->water_sources = vector<WaterSource *>(water_sources.size());

    for (int i : water_sources_ids) {
        this->water_sources[i] = water_sources[i];
    }

    this->catchments = vector<Catchment *>(catchments.size());

    for (int i : catchments_ids) {
        this->catchments[i] = catchments[i];
    }

    this->utilities = vector<Utility *>(utilities.size());

    for (int i : utilities_ids) {
        this->utilities[i] = utilities[i];
    }
}
