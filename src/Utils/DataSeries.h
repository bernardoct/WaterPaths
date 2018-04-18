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

    vector<double> *series_x;
    vector<double> *series_y;

public:
    const unsigned long length;

    DataSeries(vector<double> *series_x, vector<double> *series_y);

//    double get_dependent_variable(double x);

    const vector<double> &getSeries_x() const;

    double get_dependent_variable(double x) override;

    double get_dependent_variable(int x) override;

private:
    double get_dependent_variable(double x, int week) override;

    double get_dependent_variable(int x, int week) override;

};


#endif //TRIANGLEMODEL_DATASERIES_H
