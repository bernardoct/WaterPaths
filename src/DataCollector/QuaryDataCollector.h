//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_QUARYDATACOLLECTOR_H
#define TRIANGLEMODEL_QUARYDATACOLLECTOR_H


#include "ReservoirDataCollector.h"

class QuaryDataCollector : public ReservoirDataCollector {
public:
    QuaryDataCollector(Reservoir *reservoir, unsigned long realization);

};


#endif //TRIANGLEMODEL_QUARYDATACOLLECTOR_H
