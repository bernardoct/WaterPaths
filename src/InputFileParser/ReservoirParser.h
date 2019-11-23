//
// Created by bernardo on 11/21/19.
//

#ifndef TRIANGLEMODEL_RESERVOIRPARSER_H
#define TRIANGLEMODEL_RESERVOIRPARSER_H


#include "Base/WaterSourceParser.h"
#include "CatchmentParser.h"
#include "EvaporationSeriesParser.h"


class ReservoirParser : public WaterSourceParser {
private:
    unsigned long BLOCK_LEN_EXISTING_RESERVOIR = 7;
    unsigned long BLOCK_LEN_NON_EXISTING_RESERVOIR = 10;

    CatchmentParser catchment_parser;
    EvaporationSeriesParser evaporation_series_parser;
    EvaporationSeries evaporation_series;

    double storage_area = NON_INITIALIZED;
    double max_treatment_capacity = NON_INITIALIZED;
    bool variable_area = false;

    vector<Catchment *> catchments;


    int constructor_type = NON_INITIALIZED;

    static vector<DataSeries> storage_vs_area_curves;

public:
    void parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks) override;

    WaterSource* generateSource(vector<vector<string>> &block, int n_realizations, int n_weeks) override;


};


#endif //TRIANGLEMODEL_RESERVOIRPARSER_H
