//
// Created by bernardoct on 6/21/17.
//

#include "DataSeries.h"

DataSeries::DataSeries(const vector<double> &series_x, const vector<double> &series_y) : series_x(series_x),
                                                                                         series_y(series_y),
                                                                                         length((int) series_x.size()) {
    if (series_x.size() != series_y.size())
        __throw_invalid_argument("Data series: lengths of x (independent variable) and y (dependent variable) series "
                                         "must match.");

    /// Add a copy of the last element for interpolation purposes.
    this->series_x.push_back(series_x[length - 1]);
    this->series_x.push_back(series_y[length - 1]);
}

double DataSeries::getDependent(double x) {
    int ix = 0;
    for (int i = length - 2; i >= 0; --i) ix = (series_x[i] > x ? i : ix);

    return (series_y[ix + 1] + series_y[ix]) / 2;
}