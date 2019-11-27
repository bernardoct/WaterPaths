//
// Created by bernardo on 11/21/19.
//

#ifndef TRIANGLEMODEL_CATCHMENTPARSER_H
#define TRIANGLEMODEL_CATCHMENTPARSER_H


#include "../SystemComponents/Catchment.h"

class CatchmentParser {
    vector<vector<vector<double>>> inflow_time_series;
    vector<Catchment *> parsed_catchments;

public:
    virtual ~CatchmentParser();

    void parseSeries(vector<string> paths, int n_weeks = NON_INITIALIZED,
                     int n_realizations = NON_INITIALIZED);

    const vector<Catchment *> &getParsedCatchments() const;

};


#endif //TRIANGLEMODEL_CATCHMENTPARSER_H
