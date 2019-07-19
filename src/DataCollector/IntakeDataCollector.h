//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_INTAKEDATACOLLECTOR_H
#define TRIANGLEMODEL_INTAKEDATACOLLECTOR_H


#include "../SystemComponents/WaterSources/Intake.h"
#include "Base/DataCollector.h"

class IntakeDataCollector : public DataCollector {

    Intake *intake;
    vector<double> total_upstream_sources_inflows;
    vector<double> demands;
    vector<double> wastewater_inflows;
    vector<double> outflows;
    vector<double> total_catchments_inflow;

public:
    IntakeDataCollector(Intake *intake, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_INTAKEDATACOLLECTOR_H
