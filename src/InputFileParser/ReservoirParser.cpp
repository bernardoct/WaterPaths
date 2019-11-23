//
// Created by bernardo on 11/21/19.
//

#include "ReservoirParser.h"
#include "CatchmentParser.h"
#include "../Utils/Utils.h"

void ReservoirParser::parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks) {
    WaterSourceParser::parseVariables(block, n_realizations, n_weeks);

    for (vector<string> &line : block) {
        if (line[0] == "catchment") {
            for (auto it = begin(line) + 1; it != end(line); ++it) {
                catchments.push_back(&catchment_parser.parseSeries(*it, n_weeks, n_realizations));
            }
        } else if (line[0] == "max_treatment_capacity") {
            max_treatment_capacity = stod(line[1]);
        } else if (line[0] == "evaporation_file") {
            if (line.size() > 2) throw invalid_argument("Only one evaporation series matrix allowed per reservoir.");
            evaporation_series = EvaporationSeriesParser::parseSeries(line[1], n_weeks, n_realizations);
        } else if (line[0] == "storage_area_curve") {
            vector<double> area_curve;
            Utils::tokenizeString(line[1], area_curve, ',');
            vector<double> storage_curve;
            Utils::tokenizeString(line[2], area_curve, ',');

            storage_vs_area_curves.emplace_back(area_curve, area_curve);
            variable_area = true;
        } else if (line[0] == "storage_area") {
            storage_area = stod(line[1]);
        }
    }
}

WaterSource *ReservoirParser::generateSource(vector<vector<string>> &block, int n_realizations, int n_weeks) {
    updateId();

    parseVariables(block, n_realizations, n_weeks);

    if (block.size() == BLOCK_LEN_EXISTING_RESERVOIR && variable_area) {
        return new Reservoir(name, getId(), catchments, capacity,
                max_treatment_capacity, evaporation_series,
                             storage_vs_area_curves.back());
    } else if (block.size() == BLOCK_LEN_EXISTING_RESERVOIR) {
        return new Reservoir(name, getId(), catchments, capacity,
                             max_treatment_capacity, evaporation_series, storage_area);
    } else if (block.size() == BLOCK_LEN_EXISTING_RESERVOIR && variable_area) {
        return new Reservoir(name, getId(), catchments, capacity,
                  max_treatment_capacity, evaporation_series,
                  storage_vs_area_curves.back(), construction_time,
                             permitting_time, *bonds.back());
    } else if (block.size() == BLOCK_LEN_EXISTING_RESERVOIR) {
        return new Reservoir(name, getId(), catchments, capacity,
                  max_treatment_capacity, evaporation_series, storage_area, construction_time,
                  permitting_time, *bonds.back());
    } else {
        throw invalid_argument("Reservoir with wrong number of parameters (must be either 7 or 10).");
    }
}
