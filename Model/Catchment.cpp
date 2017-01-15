//
// Created by bernardo on 1/13/17.
//

#include "Catchment.h"
#include "Aux.h"

Catchment::Catchment(char *file_name, int number_of_records, int number_of_time_steps) {

    streamflows = Aux::parse2DCsvFile(file_name, number_of_records, number_of_time_steps);

}

double *Catchment::getStreamflow(int index) const {
    return streamflows[index];
}

