//
// Created by bernardoct on 6/21/17.
//

#ifndef TRIANGLEMODEL_DATASERIES_H
#define TRIANGLEMODEL_DATASERIES_H

#include <vector>

using namespace std;

class DataSeries {
private:
    vector<double> series_x;
    vector<double> series_y;

public:
    const int length;

    DataSeries(const vector<double> &series_x, const vector<double> &series_y);

    double getDependent(double x);
};


#endif //TRIANGLEMODEL_DATASERIES_H
