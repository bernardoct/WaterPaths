//
// Created by bernardoct on 6/21/17.
//

#ifndef TRIANGLEMODEL_DATASERIES_H
#define TRIANGLEMODEL_DATASERIES_H

#include <vector>
#include "../Controls/Base/ControlRules.h"

using namespace std;

class DataSeries : public ControlRules {
private:

    vector<float> *series_x;
    vector<float> *series_y;

public:
    const unsigned long length;

    DataSeries(vector<float> *series_x, vector<float> *series_y);

//    float get_dependent_variable(float x);

    const vector<float> &getSeries_x() const;

    float get_dependent_variable(float x) override;

    float get_dependent_variable(int x) override;

private:
    float get_dependent_variable(float x, int week) override;

    float get_dependent_variable(int x, int week) override;

};


#endif //TRIANGLEMODEL_DATASERIES_H
