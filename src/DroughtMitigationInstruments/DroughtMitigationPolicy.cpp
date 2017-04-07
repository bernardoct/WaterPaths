//
// Created by bernardo on 2/6/17.
//

#include "DroughtMitigationPolicy.h"


DroughtMitigationPolicy::DroughtMitigationPolicy(const int id, const int type) : id(id), type(type) {}

const vector<int> &DroughtMitigationPolicy::getUtilities_ids() const {
    return utilities_ids;
}




