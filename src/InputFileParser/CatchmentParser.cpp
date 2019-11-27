//
// Created by bernardo on 11/21/19.
//

#include "CatchmentParser.h"
#include "../Utils/Utils.h"


void CatchmentParser::parseSeries(vector<string> paths, int n_weeks, int n_realizations) {
    if (n_weeks == NON_INITIALIZED) {
        throw invalid_argument("Number of weeks to be parsed must be provided to catchment parser.");
    }
    for (string &path : paths) {
        inflow_time_series.push_back(Utils::parse2DCsvFile(path, n_realizations));
    }

    for (auto &series : inflow_time_series) {
        parsed_catchments.push_back(new Catchment(series,
                (int) (Constants::WEEKS_ROF_LONG_TERM *
                Constants::WEEKS_IN_YEAR) + n_weeks));
    }
}

const vector<Catchment *> &CatchmentParser::getParsedCatchments() const {
    return parsed_catchments;
}

CatchmentParser::~CatchmentParser() {
    for (auto c : parsed_catchments) {
        delete c;
    }
}
