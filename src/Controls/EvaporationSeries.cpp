//
// Created by bernardoct on 6/22/17.
//

#include "EvaporationSeries.h"

EvaporationSeries::EvaporationSeries(
        vector<vector<double>> *evaporation_series, int series_length)
        : Catchment(evaporation_series,
                    series_length) {}

/**
 * Copy constructor.
 * @param catchment
 */
EvaporationSeries::EvaporationSeries(const EvaporationSeries &evaporation_series)
        : Catchment(evaporation_series) {}

/**
 * Copy assignment operator.
 * @param catchment
 * @return
 */
EvaporationSeries &EvaporationSeries::operator=(
        const EvaporationSeries
        &evaporation_series) {

    streamflows_all = evaporation_series.streamflows_all;
    series_length = evaporation_series.series_length;
    streamflows_realization = evaporation_series.streamflows_realization;

    return *this;
}

EvaporationSeries::~EvaporationSeries() {}

double EvaporationSeries::getEvaporation(int week) {
    return Catchment::getStreamflow(week);
}

void EvaporationSeries::setRealization(unsigned long r, vector<double> &rdm_factors) {
    Catchment::setRealization(r, rdm_factors);

    for (unsigned long w = 0; w < streamflows_realization.size(); ++w) {
        streamflows_realization[w] *= rdm_factors.at(0);
    }
}
