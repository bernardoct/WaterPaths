//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_EMPTYDATACOLLECTOR_H
#define TRIANGLEMODEL_EMPTYDATACOLLECTOR_H


#include "Base/DataCollector.h"

class EmptyDataCollector : public DataCollector {
public:
    EmptyDataCollector();

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_EMPTYDATACOLLECTOR_H
