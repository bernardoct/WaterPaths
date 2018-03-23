//
// Created by bernardo on 2/3/17.
//

#ifndef TRIANGLEMODEL_RESTRICTIONS_H
#define TRIANGLEMODEL_RESTRICTIONS_H


#include "../SystemComponents/Utility.h"
#include "Base/DroughtMitigationPolicy.h"

class Restrictions : public DroughtMitigationPolicy {

private:
    vector<double> stage_multipliers;
    const vector<double> stage_triggers;
    double current_multiplier = 0;
    vector<vector<double>> restricted_weekly_average_volumetric_price;

public:
    Restrictions(const int id, const vector<double> &stage_multipliers,
                 const vector<double> &stage_triggers);

    Restrictions(
            const int id, const vector<double> &stage_multipliers,
            const vector<double> &stage_triggers,
            const vector<vector<double>> *typesMonthlyDemandFraction,
            const vector<vector<double>> *typesMonthlyWaterPrice,
            const vector<vector<double>> *priceMultipliers);

    Restrictions(const Restrictions &reservoir);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> systems_utilities,
                                 vector<WaterSource *> water_sources,
                                 vector<MinEnvFlowControl *> min_env_flow_controls) override;

    double getCurrent_multiplier() const;

    ~Restrictions();

    void calculateWeeklyAverageWaterPrices(
            const vector<vector<double>> *typesMonthlyDemandFraction,
            const vector<vector<double>> *typesMonthlyWaterPrice,
            const vector<vector<double>> *priceMultipliers);

    void setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                        vector<double> &water_sources_rdm, vector<double> &policy_rdm) override;
};


#endif //TRIANGLEMODEL_RESTRICTIONS_H
