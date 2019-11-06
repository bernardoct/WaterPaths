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
            const vector<UtilitiesDataCollector *>& utility_collector,
            vector<unsigned long> realizations = vector<unsigned long>(0));

    static double calculateRestrictionFrequencyObjective(
            const vector<RestrictionsDataCollector *>& restriction_data,
            vector<unsigned long> realizations = vector<unsigned long>(0));

    static double calculateNetPresentCostInfrastructureObjective(
            const vector<UtilitiesDataCollector *>& utility_data,
            vector<unsigned long> realizations = vector<unsigned long>(0));

    static double calculatePeakFinancialCostsObjective(
            const vector<UtilitiesDataCollector *>& utility_data,
            vector<unsigned long> realizations = vector<unsigned long>(0));

    static double calculateWorseCaseCostsObjective(
            const vector<UtilitiesDataCollector *>& utility_data,
            vector<unsigned long> realizations = vector<unsigned long>(0));

    static double calculateUnitTotalCostObjective(
            const vector<UtilitiesDataCollector *>& utility_data,
            vector<unsigned long> realizations = vector<unsigned long>(0));

    static double calculateNetPresentCostInfrastructureObjectiveForVariableDebtService(
            const vector<UtilitiesDataCollector *>& utility_data,
            vector<unsigned long> realizations = vector<unsigned long>(0));
};


#endif //TRIANGLEMODEL_OBJECTIVESCALCULATOR_H
