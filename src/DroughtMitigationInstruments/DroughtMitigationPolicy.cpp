//
// Created by bernardo on 2/6/17.
//

#include "DroughtMitigationPolicy.h"


DroughtMitigationPolicy::DroughtMitigationPolicy(const int id) : id(id) {}

const vector<int> &DroughtMitigationPolicy::getUtilities_ids() const {
    return utilities_ids;
}




