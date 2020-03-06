//
// Created by dgorelic on 3/2/2020.
//

#ifndef TRIANGLEMODEL_ALLOCATEDINTAKEDATACOLLECTOR_H
#define TRIANGLEMODEL_ALLOCATEDINTAKEDATACOLLECTOR_H


#include "Base/DataCollector.h"
#include "../SystemComponents/WaterSources/AllocatedIntake.h"

class AllocatedIntakeDataCollector : public DataCollector {

    AllocatedIntake *intake;
    vector<double> total_upstream_sources_inflows;
    vector<double> demands;
    vector<double> wastewater_inflows;
    vector<double> outflows;
    vector<double> total_catchments_inflow;
    vector<vector<double>> allocated_demands;
    vector<vector<double>> allocated_supply_capacity;
    vector<int> utilities_with_allocations;

public:
    AllocatedIntakeDataCollector(AllocatedIntake *intake, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_ALLOCATEDINTAKEDATACOLLECTOR_H
