//
// Created by bernardoct on 8/26/17.
//

#include "QuaryDataCollector.h"

QuaryDataCollector::QuaryDataCollector(Reservoir *reservoir, unsigned long realization)
        : ReservoirDataCollector(reservoir, QUARRY, 7 * COLUMN_WIDTH, realization) {}
