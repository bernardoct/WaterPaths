//
// Created by bernardo on 11/21/19.
//

#ifndef TRIANGLEMODEL_EVAPORATIONSERIESPARSER_H
#define TRIANGLEMODEL_EVAPORATIONSERIESPARSER_H


#include "../Controls/EvaporationSeries.h"

class EvaporationSeriesParser {
    vector<vector<vector<double>>> parsed_evaporation_series_data;
    vector<EvaporationSeries> parsed_evaporation_series;

public:
    EvaporationSeries &parseSeries(string path, int n_weeks, int n_realizations);

};


#endif //TRIANGLEMODEL_EVAPORATIONSERIESPARSER_H
