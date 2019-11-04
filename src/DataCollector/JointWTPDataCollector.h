//
// Created by dgorelic on 11/4/2019.
//

#ifndef TRIANGLEMODEL_JOINTWTPDATACOLLECTOR_H
#define TRIANGLEMODEL_JOINTWTPDATACOLLECTOR_H


#include "Base/DataCollector.h"
#include "../SystemComponents/WaterSources/JointWTP.h"

class JointWTPDataCollector : public DataCollector {

    JointWTP *joint_wtp;
    vector<int> utilities_with_allocations;
    vector<vector<double>> partner_allocated_treatment_capacities;

public:
    explicit JointWTPDataCollector(JointWTP *joint_wtp, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_JOINTWTPDATACOLLECTOR_H
