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
    vector<vector<double>> *rdm_factors_all;
    double *rdm_factors_realization;
    int series_length;
    // Number of historical years of data - used to set week delta_week to week 0.
    int delta_week = (int) std::round(
            Constants::WEEKS_IN_YEAR * Constants::NUMBER_REALIZATIONS_ROF);
 

public:
    Catchment(vector<vector<double>> *streamflows_all, int series_length);

    Catchment(Catchment &catchment);

    virtual ~Catchment();

    Catchment &operator=(const Catchment &catchment);

    double getStreamflow(int week);

    virtual void setRealization(unsigned long r, vector<vector<double>> *rdm_factors);
};


#endif //TRIANGLEMODEL_CATCHMENT_H
