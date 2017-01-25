//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_CATCHMENT_H
#define TRIANGLEMODEL_CATCHMENT_H

#include <vector>

using namespace std;


class Catchment {
public:
    Catchment(double *streamflows);

    double getStreamflow(int week);

private:
    double *streamflows;
};


#endif //TRIANGLEMODEL_CATCHMENT_H
