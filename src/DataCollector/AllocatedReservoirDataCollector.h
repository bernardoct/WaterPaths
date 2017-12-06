//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_ALLOCATEDRESERVOIRDATACOLLECTOR_H
#define TRIANGLEMODEL_ALLOCATEDRESERVOIRDATACOLLECTOR_H


#include "ReservoirDataCollector.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"

class AllocatedReservoirDataCollector : public ReservoirDataCollector {
    AllocatedReservoir *allocated_reservoir;
    vector<vector<double>> allocated_stored_volumes;

public:
    AllocatedReservoirDataCollector(
                AllocatedReservoir *allocated_reservoir, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_ALLOCATEDRESERVOIRDATACOLLECTOR_H
