//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_OBJECTIVESCALCULATOR_H
#define TRIANGLEMODEL_OBJECTIVESCALCULATOR_H


#include "../DataCollector/UtilitiesDataCollector.h"
#include "../DataCollector/RestrictionsDataCollector.h"

class ObjectivesCalculator {

public:
    static float calculateReliabilityObjective(
            vector<UtilitiesDataCollector *>
            utility_collector);

    static float calculateRestrictionFrequencyObjective(
            vector<RestrictionsDataCollector *> restriction_data);

    static float calculateNetPresentCostInfrastructureObjective(
            vector<UtilitiesDataCollector *>
            utility_data);

    static float calculatePeakFinancialCostsObjective(
            vector<UtilitiesDataCollector *> utility_data);

    static float calculateWorseCaseCostsObjective(
            vector<UtilitiesDataCollector *> utility_data);

};


#endif //TRIANGLEMODEL_OBJECTIVESCALCULATOR_H
