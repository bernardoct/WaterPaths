//
// Created by bernardoct on 6/21/17.
//

#include "DataSeries.h"
#include "Constants.h"

DataSeries::DataSeries(vector<double> &series_x, vector<double> &series_y) : series_x(series_x),
                                                                             series_y(series_y),
                                                                             length(series_x.size()) {
    if (series_x.size() != series_y.size())
        throw invalid_argument("Data series: lengths of x (independent variable) and y (dependent variable) series "
                                         "must match.");

    // Add a copy of the last element for interpolation purposes.
    this->series_x.push_back((double &&) series_x.at(length - 1));
    this->series_y.push_back((double &&) series_y.at(length - 1));
}

DataSeries::DataSeries() : length(Constants::NON_INITIALIZED) {}

DataSeries::DataSeries(DataSeries const &data_series) : length(data_series.length),
            series_x(data_series.series_x), series_y(data_series.series_y) {}

DataSeries &DataSeries::operator=(const DataSeries &data_series) {
    length = data_series.length;
    series_x = data_series.series_x;
    series_y = data_series.series_y;
    return *this;
}

/**
 * Get the value of y (dependent variable) for a corresponding x (independent variable).
 * @param x
 * @return
 */
double DataSeries::get_dependent_variable(double x) {

    // if the series has only one point, return that point; otherwise, interpolate.
    if (length > 1) {
        unsigned long ix = 1;
        for (unsigned long i = length - 1; i > 0; --i) ix = (series_x.at(i) >= x ? i : ix);

        return series_y.at(ix - 1) +
               (series_y.at(ix) - series_y.at(ix - 1)) * (x - series_x.at(ix - 1)) /
               (series_x.at(ix) - series_x.at(ix - 1));
    } else {
        return series_y.at(0);
    }
}

const vector<double> &DataSeries::getSeries_x() const {
    return series_x;
}

double DataSeries::get_dependent_variable(int x) {
    throw invalid_argument("Data series get_dependent_variable cannot be called with ints");
}

double DataSeries::get_dependent_variable(double x, int week) {
    throw invalid_argument("Data series get_dependent_variable cannot be called two independent variables.");
}

double DataSeries::get_dependent_variable(int x, int week) {
    throw invalid_argument("Data series get_dependent_variable cannot be called two independent variables.");
}

