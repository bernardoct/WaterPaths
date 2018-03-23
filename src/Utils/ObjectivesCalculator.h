//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_OBJECTIVESCALCULATOR_H
#define TRIANGLEMODEL_OBJECTIVESCALCULATOR_H


#include "../DataCollector/UtilitiesDataCollector.h"
#include "../DataCollector/RestrictionsDataCollector.h"

class ObjectivesCalculator {

public:
    static double calculateReliabilityObjective(
            vector<UtilitiesDataCollector *>&
            utility_collector);

    static double calculateRestrictionFrequencyObjective(
            vector<RestrictionsDataCollector>& restriction_data);

    static double calculateNetPresentCostInfrastructureObjective(
            vector<UtilitiesDataCollector *>&
            utility_data);

    static double calculatePeakFinancialCostsObjective(
            vector<UtilitiesDataCollector *>& utility_data);

    static double calculateWorseCaseCostsObjective(
            vector<UtilitiesDataCollector *>& utility_data);

};


#endif //TRIANGLEMODEL_OBJECTIVESCALCULATOR_H
