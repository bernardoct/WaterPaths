//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_QUARYDATACOLLECTOR_H
#define TRIANGLEMODEL_QUARYDATACOLLECTOR_H


#include "ReservoirDataCollector.h"

class QuaryDataCollector : public ReservoirDataCollector {
private:
    int N_COLUMNS = 7;
public:
    QuaryDataCollector(Reservoir *reservoir);

};


#endif //TRIANGLEMODEL_QUARYDATACOLLECTOR_H
