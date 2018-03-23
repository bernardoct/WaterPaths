//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_RESERVOIRDATACOLLECTOR_H
#define TRIANGLEMODEL_RESERVOIRDATACOLLECTOR_H


#include <vector>
#include "Base/DataCollector.h"
#include "../SystemComponents/WaterSources/Reservoir.h"

class ReservoirDataCollector : public DataCollector {

    Reservoir *reservoir;
    vector<double> stored_volume;
    vector<double> total_upstream_sources_inflows;
    vector<double> wastewater_inflows;
    vector<double> demands;
    vector<double> outflows;
    vector<double> total_catchments_inflow;
    vector<double> evaporated_volume;
    vector<double> area;
    bool fixed_area;
    double fixed_area_value;

public:
    explicit ReservoirDataCollector(Reservoir *reservoir, unsigned long realization);

    explicit ReservoirDataCollector(Reservoir *reservoir, int type, int table_width,
                                        unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_RESERVOIRDATACOLLECTOR_H
