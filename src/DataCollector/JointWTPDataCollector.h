//
// Created by David on 5/6/2018.
//

#ifndef TRIANGLEMODEL_JOINTWTPDATACOLLECTOR_H
#define TRIANGLEMODEL_JOINTWTPDATACOLLECTOR_H


#include "AllocatedReservoirDataCollector.h"
#include "../SystemComponents/WaterSources/JointWTP.h"

class JointWTPDataCollector : public AllocatedReservoirDataCollector {
    JointWTP *joint_wtp;
    AllocatedReservoir *parent_allocated_reservoir;

    vector<vector<double>> wtp_allocated_treatment_fractions;
    vector<vector<double>> wtp_allocated_treatment_capacities;
    vector<int> wtp_utilities_with_allocations;

    vector<vector<double>> reservoir_allocated_treatment_fractions;
    vector<vector<double>> reservoir_allocated_treatment_capacities;
    vector<int> reservoir_utilities_with_allocations;

    vector<double> wtp_external_allocated_treatment_fraction;

    int wtp_id;
    int parent_source_id;

public:
    JointWTPDataCollector(JointWTP *wtp, AllocatedReservoir *allocated_reservoir, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};



#endif //TRIANGLEMODEL_JOINTWTPDATACOLLECTOR_H
