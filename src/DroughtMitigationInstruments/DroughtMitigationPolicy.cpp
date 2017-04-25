//
// Created by bernardo on 2/6/17.
//

#include "DroughtMitigationPolicy.h"


DroughtMitigationPolicy::DroughtMitigationPolicy(const int id, const int type) : id(id), type(type) {}

DroughtMitigationPolicy::DroughtMitigationPolicy(const DroughtMitigationPolicy &drought_mitigation_policy) :
        id(drought_mitigation_policy.id), type(drought_mitigation_policy.type) {
    utilities = vector<Utility *>();
//    utilities = Utils::copyUtilityVector(drought_mitigation_policy.utilities);
}

const vector<int> &DroughtMitigationPolicy::getUtilities_ids() const {
    return utilities_ids;
}

DroughtMitigationPolicy::~DroughtMitigationPolicy() {
    utilities.clear();
}

bool DroughtMitigationPolicy::operator<(const DroughtMitigationPolicy *other) {
    return id < other->id;
}

bool DroughtMitigationPolicy::operator>(const DroughtMitigationPolicy *other) {
    return id > other->id;
}




