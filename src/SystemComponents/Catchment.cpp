//
// Created by bernardo on 1/13/17.
//

#include <cmath>
#include "Catchment.h"
#include "../Utils/Constants.h"


Catchment::Catchment(vector<vector<double>> *streamflows_all, int series_length)
        : streamflows_all(streamflows_all), series_length(series_length) {

    if (series_length <
        Constants::WEEKS_IN_YEAR * Constants::NUMBER_REALIZATIONS_ROF)
        throw std::length_error("Series shorter than required for ROF "
                                        "calculations. The streamflow series"
                                        "must be greater the number of ROF ("
                                + to_string(Constants::NUMBER_REALIZATIONS_ROF)
                                + ") realizations times the number of "
                                        "weeks in a year ("
                                + to_string(Constants::WEEKS_IN_YEAR) + ").");
}

/**
 * Copy constructor.
 * @param catchment
 */
Catchment::Catchment(Catchment &catchment) :
        series_length(catchment.series_length),
        streamflows_all(catchment.streamflows_all),
        streamflows_realization(catchment.streamflows_realization) {
}

/**
 * Copy assignment operator.
 * @param catchment
 * @return
 */
Catchment &Catchment::operator=(const Catchment &catchment) = default;

/**
 * Destructor.
 */
Catchment::~Catchment() {
//    streamflows_all = nullptr;
}

/**
 * Get streamflow for a given week. This function assures that the number of
 * past inflows used for ROF calculations are reserved in the beginning of
 * the time series for ROF calculations.
 * @param week
 * @return
 */
double Catchment::getStreamflow(int week) {
    int adjusted_week = week + delta_week;
    return streamflows_realization[adjusted_week];
}

/**
 * Get time series corresponding to realization index and eliminate reference to
 * comprehensive streamflow
 * data set.
 * @param r
 */
void Catchment::setRealization(unsigned long r, vector<vector<double>> *rdm_factors) {
    streamflows_realization = new double[streamflows_all->at(r).size()];
    std::copy(streamflows_all->at(r).begin(),
              streamflows_all->at(r).end(),
              streamflows_realization);
}

