//
// Created by bernardoct on 6/22/17.
//

#ifndef TRIANGLEMODEL_EVAPORATIONSERIES_H
#define TRIANGLEMODEL_EVAPORATIONSERIES_H

#include <vector>
#include "../SystemComponents/Catchment.h"

class EvaporationSeries : public Catchment {
public:
    EvaporationSeries(
            vector<vector<double>> *evaporation_series, int series_length);

    EvaporationSeries(const EvaporationSeries &evaporation_series);

    EvaporationSeries &operator=(const EvaporationSeries &evaporation_series);

    ~EvaporationSeries() override;

    double getEvaporation(int week);

    void setRealization(unsigned long r, vector<double> &rdm_factors) override;
};


#endif //TRIANGLEMODEL_EVAPORATIONSERIES_H
