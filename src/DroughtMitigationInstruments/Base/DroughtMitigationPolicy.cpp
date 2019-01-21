//
// Created by bernardo on 2/6/17.
//

#include "DroughtMitigationPolicy.h"


DroughtMitigationPolicy::DroughtMitigationPolicy(const int id, const int type) : id(id), type(type) {}

DroughtMitigationPolicy::DroughtMitigationPolicy(const DroughtMitigationPolicy &drought_mitigation_policy) :
        id(drought_mitigation_policy.id),
        type(drought_mitigation_policy.type) {
//    realization_utilities = vector<Utility *>();
//    if (!realization_utilities.empty())
//        __throw_invalid_argument("Your vector of Utility pointers must be empty for you to copy it, otherwise this"
//                                         "policy will act either on the wrong realization or on a freed pointer");
}

const vector<int> &DroughtMitigationPolicy::getUtilities_ids() const {
    return utilities_ids;
}

DroughtMitigationPolicy::~DroughtMitigationPolicy() {}

bool DroughtMitigationPolicy::operator<(const DroughtMitigationPolicy *other) {
    return id < other->id;
}

bool DroughtMitigationPolicy::operator>(const DroughtMitigationPolicy *other) {
    return id > other->id;
}

double DroughtMitigationPolicy::getRofFromRealizationTable(int utility_id, int week, int tier) {
    return (*DroughtMitigationPolicy::storage_to_rof_table_)[utility_id](week, tier);
}

void DroughtMitigationPolicy::setStorage_to_rof_table_(vector<Matrix2D<double>> &storage_to_rof_table_,
                                                       int use_imported_tables) {
    DroughtMitigationPolicy::storage_to_rof_table_ = &storage_to_rof_table_;
    DroughtMitigationPolicy::use_imported_tables = use_imported_tables == IMPORT_ROF_TABLES;
}




