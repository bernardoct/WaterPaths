//
// Created by bernardo on 11/21/19.
//

#include "EvaporationSeriesParser.h"
#include "../Utils/Utils.h"


EvaporationSeries &
EvaporationSeriesParser::parseSeries(vector<vector<double>> *series,
                                     int n_weeks, int n_realizations) {
    parsed_evaporation_series.emplace_back(
            *series,
            (int) (Constants::WEEKS_ROF_LONG_TERM * Constants::WEEKS_IN_YEAR) +
            n_weeks);

    return parsed_evaporation_series.back();
}

EvaporationSeriesParser::~EvaporationSeriesParser() = default;
