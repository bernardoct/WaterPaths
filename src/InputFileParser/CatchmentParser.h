//
// Created by bernardo on 11/21/19.
//

#ifndef TRIANGLEMODEL_CATCHMENTPARSER_H
#define TRIANGLEMODEL_CATCHMENTPARSER_H


#include "../SystemComponents/Catchment.h"

class CatchmentParser {
    vector<vector<vector<double>>> parsed_inflow_csv_matrices;
    vector<Catchment> parsed_inflow_series;

public:
    Catchment &parseSeries(string path, int n_weeks, int n_realizations);

};


#endif //TRIANGLEMODEL_CATCHMENTPARSER_H
