//
// Created by bernardoct on 6/21/17.
//

#ifndef TRIANGLEMODEL_DATASERIES_H
#define TRIANGLEMODEL_DATASERIES_H

#include <vector>

using namespace std;

class DataSeries {
private:

    vector<double> *series_x;
    vector<double> *series_y;

public:
    const unsigned long length;

    DataSeries(vector<double> *series_x, vector<double> *series_y);

    double getDependent(double x);

    const vector<double> &getSeries_x() const;

};


#endif //TRIANGLEMODEL_DATASERIES_H
