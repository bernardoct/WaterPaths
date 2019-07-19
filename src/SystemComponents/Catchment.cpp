//
// Created by bernardo on 1/13/17.
//

#include <cmath>
#include "Catchment.h"


Catchment::Catchment(vector<vector<double>> *streamflows_all, int series_length)
        : streamflows_all(streamflows_all), series_length(series_length) {

    if (series_length <
        Constants::WEEKS_IN_YEAR * Constants::NUMBER_REALIZATIONS_ROF)
        throw std::length_error("Series in catchment is " 
			        + to_string(series_length) + " weeks long and "
					"therefore shorter than required for ROF"
					" calculations. The streamflow series "
					"must be greater the number of ROF ("
                                + to_string(Constants::NUMBER_REALIZATIONS_ROF)
                                + ") realizations times the number of "
                                        "weeks in a year ("
                                + to_string(Constants::WEEKS_IN_YEAR) + ").");

    if (streamflows_all->empty() || streamflows_all->at(0).empty()) {
        throw std::length_error("Empty time series.");
    }
}

/**
 * Copy constructor.
 * @param catchment
 */
Catchment::Catchment(const Catchment &catchment) :
        streamflows_all(catchment.streamflows_all),
        streamflows_realization(catchment.streamflows_realization),
        series_length(catchment.series_length),
        parent(false) {}

/**
 * Copy assignment operator.
 * @param catchment
 * @return
 */
Catchment &Catchment::operator=(const Catchment &catchment) = default;

/**
 * Destructor.
 */
Catchment::~Catchment() {}

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
//    return streamflows_realization.at((unsigned long) adjusted_week);
}

/**
 * Get time series corresponding to realization index and eliminate reference to
 * comprehensive streamflow
 * data set.
 * @param r
 */
void Catchment::setRealization(unsigned long r, vector<double> &rdm_factors) {
    streamflows_realization = vector<double>(streamflows_all->at(r));
}

