//
// Created by bernardoct on 6/21/17.
//

#ifndef TRIANGLEMODEL_DATASERIES_H
#define TRIANGLEMODEL_DATASERIES_H

#include <vector>
#include <stdexcept>
#include "../Controls/Base/ControlRules.h"

using namespace std;

class DataSeries : public ControlRules {
private:

    vector<double> series_x;
    vector<double> series_y;
    unsigned long length;

    double get_dependent_variable(double x, int week) override;

    double get_dependent_variable(int x, int week) override;

public:

    DataSeries(vector<double> &series_x, vector<double> &series_y);

    DataSeries();

    DataSeries(DataSeries const &data_series);

    DataSeries &operator=(const DataSeries &data_series);

    const vector<double> &getSeries_x() const;

    double get_dependent_variable(double x) override;

    double get_dependent_variable(int x) override;

};


#endif //TRIANGLEMODEL_DATASERIES_H
