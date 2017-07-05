//
// Created by bct52 on 6/28/17.
//

#include "MinEnvironFlowControl.h"

MinEnvironFlowControl::MinEnvironFlowControl(int water_source_id,
                                             const vector<int> &water_sources_ids,
                                             const vector<int> &aux_utilities_ids, int type)
        : water_source_id(water_source_id),
          water_sources_ids(water_sources_ids),
          utilities_ids(aux_utilities_ids),
          type(type) {}

MinEnvironFlowControl::MinEnvironFlowControl(
        const MinEnvironFlowControl &min_env_control) :
        water_source_id(min_env_control.water_source_id), water_sources_ids
        (min_env_control.water_sources_ids),
        utilities_ids(min_env_control.utilities_ids),
        type(min_env_control.type){
}

void MinEnvironFlowControl::addComponents(
        vector<WaterSource *> water_sources, vector<Utility *> utilities) {
    this->water_sources = vector<WaterSource *>(water_sources.size());

    for (int i : water_sources_ids) {
        this->water_sources[i] = water_sources[i];
    }

    this->utilities = vector<Utility *>(utilities.size());

    for (int i : utilities_ids) {
        this->utilities[i] = utilities[i];
    }
}

void MinEnvironFlowControl::setRealization(unsigned int r) {

}
