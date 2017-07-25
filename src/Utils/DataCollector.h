//
// Created by bct52 on 2/1/17.
//

#ifndef TRIANGLEMODEL_DATACOLLECTOR_H
#define TRIANGLEMODEL_DATACOLLECTOR_H

#include <vector>
#include "../SystemComponents/Utility.h"
#include "../ContinuityModels/ContinuityModelRealization.h"

/*
 * One class for storage (instead of structures) and a class that just collects that data (generic infoCollector
 * class, which is extended to utilityCollector and so on). Info collector
 */

struct Utility_t {
    Utility_t(int id, const char *name) : id(id), name(name) {};

    vector<vector<double>> st_rof;
    vector<vector<double>> lt_rof;
    vector<vector<double>> combined_storage;
    vector<vector<double>> unrestricted_demand;
    vector<vector<double>> restricted_demand;
    vector<vector<double>> contingency_fund_size;
    vector<vector<double>> gross_revenues;
    vector<vector<double>> contingency_fund_contribution;
    vector<vector<double>> debt_service_payments;
    vector<vector<double>> insurance_contract_cost;
    vector<vector<double>> insurance_payout;
    vector<vector<double>> drought_mitigation_cost;
    vector<vector<double>> capacity;
    vector<double> objectives;
    vector<double> net_present_infrastructure_cost;
    int id;
    const char *name;
};

struct WaterSource_t {
    WaterSource_t(int id, double capacity, const char *name) : id(id), capacity(capacity), name(name) {};

    vector<vector<double>> available_volume;
    vector<vector<double>> total_upstream_sources_inflows;
    vector<vector<double>> demands;
    vector<vector<double>> outflows;
    vector<vector<double>> total_catchments_inflow;
    vector<vector<double>> evaporated_volume;
    double capacity;
    int id;
    const char *name;
};

struct RestrictionPolicy_t {
    RestrictionPolicy_t(int utility_id) : utility_id(utility_id) {};

    int utility_id = NON_INITIALIZED;
    vector<vector<double>> restriction_multiplier;

    bool operator<(const RestrictionPolicy_t other) { return this->utility_id < other.utility_id; };

    bool operator>(const RestrictionPolicy_t other) { return this->utility_id > other.utility_id; };
};

struct TransfersPolicy_t {
    TransfersPolicy_t(int transfer_policy_id, vector<int> utilities_ids) :
            utilities_ids(utilities_ids), transfer_policy_id(transfer_policy_id) {};

    int transfer_policy_id = NON_INITIALIZED;
    vector<int> utilities_ids;
    vector<vector<vector<double>>> demand_offsets;
};

class DataCollector {

private:
    vector<Utility_t> utilities_t;
    vector<WaterSource_t> water_sources_t;
    vector<RestrictionPolicy_t> restriction_policies_t;
    vector<TransfersPolicy_t> transfers_policies_t;
    string output_directory = "/home/bernardoct/CLionProjects/RevampedTriangleModel/TestFiles/output/";
//    string output_directory = "..\\TestFiles\\";
    vector<vector<vector<int>>> pathways;


public:
    DataCollector() : water_sources_graph(Graph(0)), number_of_realizations(NONE) {};

    DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                  const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                  int number_of_realizations, Graph water_sources_graph);

    const Graph water_sources_graph;

    const int number_of_realizations;

    void collectData(ContinuityModelRealization *continuity_model_realization);

    void printUtilityOutputCompact(string fileName);

    void printReservoirOutputCompact(string fileName);

    void printPoliciesOutputCompact(string fileName);

    void calculateObjectives();

    double calculateReliabilityObjective(Utility_t utility);

    double calculateRestrictionFrequencyObjective(RestrictionPolicy_t restriction_t);

    void printObjectives(string fileName);

    double calculateNetPresentCostInfrastructureObjective(Utility_t utility_t);

    double calculatePeakFinancialCostsObjective(Utility_t utility_t);

    double calculateWorseCaseCostsObjective(Utility_t utility_t);

    void printPathways(string file_name);

    void printReservoirOutput(string file_name);

    void printUtilityOutput(string file_name);

    void printPoliciesOutput(string file_name);
};


#endif //TRIANGLEMODEL_DATACOLLECTOR_H
