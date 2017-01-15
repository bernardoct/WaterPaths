//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_CATCHMENT_H
#define TRIANGLEMODEL_CATCHMENT_H

#include <vector>

using namespace std;


class Catchment {
public:
    Catchment(char *file_name, int number_of_records, int number_of_time_steps);

    double *getStreamflow(int index) const;

private:
    double **streamflows;
};


#endif //TRIANGLEMODEL_CATCHMENT_H
