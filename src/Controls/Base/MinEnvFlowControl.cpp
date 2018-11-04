//
// Created by bct52 on 6/28/17.
//

#include "MinEnvFlowControl.h"

MinEnvFlowControl::MinEnvFlowControl(int water_source_id,
                                             const vector<int> &water_sources_ids,
                                             const vector<int> &aux_utilities_ids, int type)
        : 
          water_sources_ids(water_sources_ids),
          utilities_ids(aux_utilities_ids),
          water_source_id(water_source_id),
          type(type) {}

MinEnvFlowControl::MinEnvFlowControl(
        const MinEnvFlowControl &min_env_control) :
        water_sources(vector<WaterSource *>()),
        utilities(vector<Utility *>()),
        water_sources_ids(min_env_control.water_sources_ids),
        utilities_ids(min_env_control.utilities_ids),
        water_source_id(min_env_control.water_source_id),
        type(min_env_control.type) {}


void MinEnvFlowControl::addComponents(
        vector<WaterSource *> water_sources, vector<Utility *> utilities) {
    this->water_sources = vector<WaterSource *>(water_sources.size());

    for (int i : water_sources_ids) {
        // Check if water source with ID assigned to minimum environmental
        // control rule exists.
        if (i >= water_sources.size()) {
            string error = "Minimum environmental flow control assigned to "
                           "non-existing water source with ID " + to_string(i);
            __throw_invalid_argument(error.c_str());
        }
        this->water_sources[i] = water_sources[i];
    }

    this->utilities = vector<Utility *>(utilities.size());

    for (int i : utilities_ids) {
        this->utilities[i] = utilities[i];
    }
}

void MinEnvFlowControl::setRealization(unsigned long r, vector<double> &rdm_factors) {

}

MinEnvFlowControl::~MinEnvFlowControl() = default;
