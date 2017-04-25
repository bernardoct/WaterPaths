//
// Created by bernardo on 1/13/17.
//

#include <cmath>
#include "Catchment.h"
#include "../Utils/Constants.h"


Catchment::Catchment(double *streamflows, int series_length) : streamflows(streamflows),
                                                               series_length(series_length) {

    if (series_length < Constants::WEEKS_IN_YEAR * Constants::NUMBER_REALIZATIONS_ROF)
        throw std::length_error("Series shorter than required for ROF calculations. The streamflow series"
                                        "must be greater the number of ROF realizations times the number"
                                        "of weeks in a year (52).");
}

/**
 * Copy constructor.
 * @param catchment
 */
Catchment::Catchment(Catchment &catchment) : series_length(catchment.series_length),
                                             streamflows(new double[catchment.series_length]) {

    std::copy(catchment.streamflows, catchment.streamflows + catchment.series_length, streamflows);
}

/**
 * Copy assignment operator.
 * @param catchment
 * @return
 */
Catchment &Catchment::operator=(const Catchment &catchment) {

    double *streamflows_temp = streamflows; // check if it's better not to initialize it.
    std::copy(catchment.streamflows, catchment.streamflows + catchment.series_length, streamflows_temp);
    delete[] streamflows;
    streamflows = streamflows_temp;

    return *this;
}

/**
 * Destructor.
 */
Catchment::~Catchment() {
    if (streamflows)
        delete[] streamflows;
}

/**
 * Get streamflow for a given week. This function assures that the number of past inflows used for ROF
 * calculations are reserved in the beginning of the time series for ROF calculations.
 * @param week
 * @return
 */
double Catchment::getStreamflow(int week) {
    return streamflows[week + (int) std::round(Constants::WEEKS_IN_YEAR * Constants::NUMBER_REALIZATIONS_ROF)];
}

