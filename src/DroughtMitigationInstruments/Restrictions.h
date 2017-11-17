//
// Created by bernardo on 2/3/17.
//

#ifndef TRIANGLEMODEL_RESTRICTIONS_H
#define TRIANGLEMODEL_RESTRICTIONS_H


#include "../SystemComponents/Utility.h"
#include "Base/DroughtMitigationPolicy.h"

class Restrictions : public DroughtMitigationPolicy {

private:
    const vector<float> stage_multipliers;
    const vector<float> stage_triggers;
    float current_multiplier = 0;
    float **restricted_weekly_average_volumetric_price = nullptr;

public:
    Restrictions(const int id, const vector<float> &stage_multipliers,
                 const vector<float> &stage_triggers);

    Restrictions(
            const int id, const vector<float> &stage_multipliers,
            const vector<float> &stage_triggers,
            const vector<vector<float>> *typesMonthlyDemandFraction,
            const vector<vector<float>> *typesMonthlyWaterPrice,
            const vector<vector<float>> *priceMultipliers);

    Restrictions(const Restrictions &reservoir);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> systems_utilities,
                                 vector<WaterSource *> water_sources,
                                 vector<MinEnvironFlowControl *> min_env_flow_controls) override;

    float getCurrent_multiplier() const;

    ~Restrictions();

    void calculateWeeklyAverageWaterPrices(
            const vector<vector<float>> *typesMonthlyDemandFraction,
            const vector<vector<float>> *typesMonthlyWaterPrice,
            const vector<vector<float>> *priceMultipliers);

    void setRealization(unsigned int realization_id) override;
};


#endif //TRIANGLEMODEL_RESTRICTIONS_H
