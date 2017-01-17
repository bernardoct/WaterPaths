//
// Created by bernardo on 1/13/17.
//

#include "Catchment.h"

Catchment::Catchment(double *streamflows) : streamflows(streamflows) {

}

double Catchment::getStreamflow(int week) {
    return streamflows[week];
}

