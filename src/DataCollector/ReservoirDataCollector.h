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
    vector<float> stored_volume;
    vector<float> total_upstream_sources_inflows;
    vector<float> wastewater_inflows;
    vector<float> demands;
    vector<float> outflows;
    vector<float> total_catchments_inflow;
    vector<float> evaporated_volume;
    vector<float> area;

public:
    explicit ReservoirDataCollector(Reservoir *reservoir);

    explicit ReservoirDataCollector(
            Reservoir *reservoir, int type,
            int table_width);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_RESERVOIRDATACOLLECTOR_H
