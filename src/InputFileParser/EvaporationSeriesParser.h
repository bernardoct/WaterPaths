//
// Created by bernardo on 11/21/19.
//

#ifndef TRIANGLEMODEL_EVAPORATIONSERIESPARSER_H
#define TRIANGLEMODEL_EVAPORATIONSERIESPARSER_H


#include "../Controls/EvaporationSeries.h"

class EvaporationSeriesParser {
    vector<EvaporationSeries> parsed_evaporation_series;

public:
    ~EvaporationSeriesParser();

    EvaporationSeries &parseSeries(vector<vector<double>> *series, int n_weeks,
                                   int n_realizations);

};


#endif //TRIANGLEMODEL_EVAPORATIONSERIESPARSER_H
