//
// Created by bernardo on 11/21/19.
//

#include "CatchmentParser.h"
#include "../Utils/Utils.h"


Catchment &CatchmentParser::parseSeries(string path, int n_weeks, int n_realizations) {
    parsed_inflow_csv_matrices.push_back(Utils::parse2DCsvFile(path, n_realizations));
    parsed_inflow_series.emplace_back(&parsed_inflow_csv_matrices.back(), n_weeks);
    return parsed_inflow_series.back();
}