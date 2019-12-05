//
// Created by bernardo on 11/21/19.
//

#include "ReservoirParser.h"
#include "../../Utils/Utils.h"
#include "../Exceptions/MissingParameter.h"
#include "../AuxParserFunctions.h"


ReservoirParser::ReservoirParser(bool generate_tables)
        : WaterSourceParser("[RESERVOIR]"), generate_tables(generate_tables) {}

ReservoirParser::ReservoirParser(string tag_name, bool generate_tables)
        : WaterSourceParser(tag_name), generate_tables(generate_tables) {}

void ReservoirParser::parseVariables(vector<vector<string>> &block,
                                     int n_realizations, int n_weeks,
                                     int line_no,
                                     const map<string, int> &ws_name_to_id,
                                     const map<string, int> &utility_name_to_id) {
    WaterSourceParser::parseVariables(block, n_realizations, n_weeks, line_no,
                                      ws_name_to_id, utility_name_to_id);

    // capacity multiplier for pre-computed rof-tables generation.
    if (generate_tables) capacity *= Constants::BASE_STORAGE_CAPACITY_MULTIPLIER;

    vector<unsigned long> rows_read;
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> &line = block[i];
        if (line[0] == "streamflow_files") {
            catchment_parser.parseSeries(
                    vector<string>(line.begin() + 1, line.end()),
                    n_weeks, n_realizations
            );
            catchments = catchment_parser.getParsedCatchments();
            rows_read.push_back(i);
        } else if (line[0] == "treatment_capacity") {
            treatment_capacity = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "evaporation_file") {
            if (line.size() > 2)
                throw invalid_argument(
                        "Only one evaporation series matrix allowed per reservoir.");
            evaporation_series = evaporation_series_parser.parseSeries(line[1],
                                                                       n_weeks,
                                                                       n_realizations);
            rows_read.push_back(i);
        } else if (line[0] == "storage_area_curve") {
            vector<double> storage_curve;
            Utils::tokenizeString(line[1], storage_curve, ',');
            vector<double> area_curve;
            Utils::tokenizeString(line[2], area_curve, ',');

            // capacity multiplier for pre-computed rof-tables generation.
            if (generate_tables) {
                for (double &s : storage_curve) s *= Constants::BASE_STORAGE_CAPACITY_MULTIPLIER;
            }

            storage_vs_area_curves = DataSeries(storage_curve, area_curve);
            variable_area = true;
            rows_read.push_back(i);
        } else if (line[0] == "storage_area") {
            storage_area = stod(line[1]);
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

WaterSource *
ReservoirParser::generateSource(int id, vector<vector<string>> &block,
                                int line_no, int n_realizations,
                                int n_weeks,
                                const map<string, int> &ws_name_to_id,
                                const map<string, int> &utility_name_to_id) {

    parseVariables(block, n_realizations, n_weeks, line_no,
                   ws_name_to_id, utility_name_to_id);
    checkMissingOrExtraParams(line_no, block);

    if (existing_infrastructure && variable_area) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series,
                             storage_vs_area_curves);
    } else if (existing_infrastructure) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series,
                             storage_area);
    } else if (!existing_infrastructure && variable_area) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series,
                             storage_vs_area_curves, construction_time,
                             permitting_time, *bonds.back());
    } else if (!existing_infrastructure) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series,
                             storage_area, construction_time,
                             permitting_time, *bonds.back());
    } else {
        throw invalid_argument("Really weird error in ReservoirParser. Please "
                               "report this to bct52@cornell.edu");
    }
}

void ReservoirParser::checkMissingOrExtraParams(int line_no,
                                                vector<vector<string>> &block) {
    WaterSourceParser::checkMissingOrExtraParams(line_no, block);

    if (evaporation_series.getSeriesLength() == NON_INITIALIZED) {
        throw MissingParameter("evaporation_series", tag_name, line_no);
    } else if (catchments.empty()) {
        throw MissingParameter("catchments", tag_name, line_no);
    } else if (storage_area == NON_INITIALIZED &&
               storage_vs_area_curves.getSeries_x().empty()) {
        throw MissingParameter("storage_area and storage_vs_area_curves "
                               "(one is needed)", tag_name, line_no);
    }
}

ReservoirParser::~ReservoirParser() = default;

