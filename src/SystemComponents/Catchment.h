//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_CATCHMENT_H
#define TRIANGLEMODEL_CATCHMENT_H

#include <vector>
#include "../Utils/Constants.h"

using namespace std;
using namespace Constants;

class Catchment {
protected:
    vector<vector<double>> *streamflows_all;
    double *streamflows_realization;
    int series_length;

public:
    Catchment(vector<vector<double>> *streamflows_all, int series_length);

    Catchment(Catchment &catchment);

    virtual ~Catchment();

    Catchment &operator=(const Catchment &catchment);

    double getStreamflow(int week);

    void setRealization(unsigned long r);
};


#endif //TRIANGLEMODEL_CATCHMENT_H
