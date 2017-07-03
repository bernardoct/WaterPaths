//
// Created by bernardoct on 6/22/17.
//

#include "EvaporationSeries.h"

EvaporationSeries::EvaporationSeries(
        vector<vector<double>> *evaporation_series, int series_length)
        : Catchment(
        evaporation_series,
        series_length) {}

/**
 * Copy constructor.
 * @param catchment
 */
EvaporationSeries::EvaporationSeries(EvaporationSeries &evaporation_series)
        : Catchment(evaporation_series) {}

/**
 * Copy assignment operator.
 * @param catchment
 * @return
 */
EvaporationSeries &
EvaporationSeries::operator=(const EvaporationSeries &evaporation_series) {

    streamflows_all = evaporation_series.streamflows_all;
    series_length = evaporation_series.series_length;
    streamflows_realization = evaporation_series.streamflows_realization;

    return *this;
}

double EvaporationSeries::getEvaporation(int week) {
    return Catchment::getStreamflow(week);
}
