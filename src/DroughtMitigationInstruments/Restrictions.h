//
// Created by bernardo on 2/3/17.
//

#ifndef TRIANGLEMODEL_RESTRICTIONS_H
#define TRIANGLEMODEL_RESTRICTIONS_H


#include "../SystemComponents/Utility.h"
#include "DroughtMitigationPolicy.h"
#include "../Utils/Graph/Graph.h"

class Restrictions : public DroughtMitigationPolicy {

private:

    const vector<double> stage_multipliers;
    const vector<double> stage_triggers;
    double current_multiplier = 0;
    int current_stage = 0;
public:

    Restrictions(const int id, const vector<double> &stage_multipliers,
                 const vector<double> &stage_triggers);

    Restrictions(const Restrictions &reservoir);

    virtual void applyPolicy(int week) override;

    virtual void addSystemComponents(vector<Utility *> systems_utilities, vector<WaterSource *> water_sources) override;

    double getCurrent_multiplier() const;

    virtual ~Restrictions();
};


#endif //TRIANGLEMODEL_RESTRICTIONS_H
