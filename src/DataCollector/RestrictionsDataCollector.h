//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_DROUGHTMITIGATIONPOLICIESDATACOLLECTOR_H
#define TRIANGLEMODEL_DROUGHTMITIGATIONPOLICIESDATACOLLECTOR_H


#include <vector>
#include "Base/DataCollector.h"
#include "../DroughtMitigationInstruments/Restrictions.h"

class RestrictionsDataCollector : public DataCollector {
private:
    Restrictions *restriction_policy;
    vector<float> restriction_multipliers;

public:
    explicit RestrictionsDataCollector(
            Restrictions *restriction_policy);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;

    const vector<float> &getRestriction_multipliers() const;
};


#endif //TRIANGLEMODEL_DROUGHTMITIGATIONPOLICIESDATACOLLECTOR_H
