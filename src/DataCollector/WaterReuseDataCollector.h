//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_WATERREUSEDATACOLLECTOR_H
#define TRIANGLEMODEL_WATERREUSEDATACOLLECTOR_H


#include "Base/DataCollector.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"

class WaterReuseDataCollector : public DataCollector {
private:
    WaterReuse *water_reuse;
    int N_COLUMNS = 2;

public:
    WaterReuseDataCollector(WaterReuse *water_reuse);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_WATERREUSEDATACOLLECTOR_H
