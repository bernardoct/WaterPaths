//
// Created by bernardo on 11/21/19.
//

#include "EvaporationSeriesParser.h"
#include "../Utils/Utils.h"


EvaporationSeries &EvaporationSeriesParser::parseSeries(string path, int n_weeks, int n_realizations) {
    parsed_evaporation_csv_matrices.push_back(Utils::parse2DCsvFile(path, n_realizations));
    parsed_evaporation_series.emplace_back(&parsed_evaporation_csv_matrices.back(), n_weeks);
    return parsed_evaporation_series.back();
}
