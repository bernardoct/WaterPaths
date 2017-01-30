//
// Created by bernardo on 1/13/17.
//

#include "Catchment.h"


Catchment::Catchment(double *streamflows, int series_length) : streamflows(streamflows),
                                                               series_length(series_length) {
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
    delete[] streamflows;
}

double Catchment::getStreamflow(int week) {
    return streamflows[week];
}

