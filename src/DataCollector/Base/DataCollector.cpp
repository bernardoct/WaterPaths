//
// Created by bernardoct on 8/25/17.
//

#include "DataCollector.h"

DataCollector::DataCollector(int id, const char *name, unsigned long realization, int type, int table_width)
        : id(id), type(type), table_width(table_width), name(name), realization(realization) {}

DataCollector::~DataCollector() {}

