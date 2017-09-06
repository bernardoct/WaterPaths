//
// Created by bernardoct on 8/26/17.
//

#include "QuaryDataCollector.h"

QuaryDataCollector::QuaryDataCollector(Reservoir *reservoir)
        : ReservoirDataCollector(reservoir,
                                 QUARRY,
                                 N_COLUMNS * COLUMN_WIDTH) {}
