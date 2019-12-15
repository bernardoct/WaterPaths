//
// Created by bernardo on 11/21/19.
//

#include "CatchmentParser.h"
#include "../Utils/Utils.h"


CatchmentParser::CatchmentParser() = default;

CatchmentParser::~CatchmentParser() {
    for (auto c : parsed_catchments) {
        delete c;
    }
}

void CatchmentParser::parseSeries(vector<vector<vector<double>> *> &series,
                                  int n_weeks, int n_realizations) {
    if (n_weeks == NON_INITIALIZED) {
        throw invalid_argument(
                "Number of weeks to be parsed must be provided to catchment parser.");
    }

    for (auto s : series) {
        parsed_catchments.push_back(new Catchment(
                *s, (int) (Constants::WEEKS_ROF_LONG_TERM *
                           Constants::WEEKS_IN_YEAR) + n_weeks));
    }
}

const vector<Catchment *> &CatchmentParser::getParsedCatchments() const {
    return parsed_catchments;
}
