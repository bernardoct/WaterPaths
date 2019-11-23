//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_DATACOLLECTOR_H
#define TRIANGLEMODEL_DATACOLLECTOR_H

#include <string>

using namespace std;

class DataCollector {
public:
    const int id;
    const int type;
    const int table_width;
    string name;
    unsigned long realization;

    explicit DataCollector(int id, string name, unsigned long realization, int type, int table_width);

    virtual ~DataCollector();

    virtual string printTabularString(int week) = 0;

    virtual string printCompactString(int week) = 0;

    virtual string printTabularStringHeaderLine1() = 0;

    virtual string printTabularStringHeaderLine2() = 0;

    virtual string printCompactStringHeader() = 0;

    virtual void collect_data() = 0;
};


#endif //TRIANGLEMODEL_DATACOLLECTOR_H
