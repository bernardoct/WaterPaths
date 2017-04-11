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
    Utility_t(int id, double capacity, string name) : id(id), capacity(capacity) {};

    vector<double> rof;
    vector<double> combined_storage;
    vector<double> unrestricted_demand;
    vector<double> restricted_demand;
    vector<double> contingency_fund_size;
    double capacity;
    int id;
    string name;
};

struct WaterSource_t {
    WaterSource_t(int id, double capacity, string name) : id(id), capacity(capacity) {};

    vector<double> available_volume;
    vector<double> total_upstream_sources_inflows;
    vector<double> demands;
    vector<double> outflows;
    vector<double> total_catchments_inflow;
    double capacity;
    int id;
    string name;
};

struct RestrictionPolicy_t {
    RestrictionPolicy_t(int utility_id) : utility_id(utility_id) {};

    int utility_id;
    vector<double> restriction_multiplier;
};

struct Transfers_policy_t {
    Transfers_policy_t(int transfer_policy_id, vector<int> buying_utilities_ids) :
            buying_utilities_ids(buying_utilities_ids), transfer_policy_id(transfer_policy_id) {};

    int transfer_policy_id;
    vector<int> buying_utilities_ids;
    vector<vector<double>> demand_offsets;
};

class DataCollector {

private:
    vector<vector<Utility_t>> utilities_t;
    vector<vector<WaterSource_t>> reservoir_t;
    vector<vector<RestrictionPolicy_t>> restriction_policy_t;
    vector<vector<Transfers_policy_t>> transfers_policy_t;
    string output_directory = "/home/bernardoct/CLionProjects/RevampedTriangleModel/TestFiles/";
//    string output_directory = "/home/bct52/CLionProjects/RevampedTriangleModel/TestFiles/";
    int number_of_realizations;

public:
    DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                  const vector<DroughtMitigationPolicy *> &drought_mitigation_policies, int number_of_realizations);

    void collectData(ContinuityModelRealization *continuity_model_realization, int week);

    void printUtilityOutput(bool toFile, string fileName = "Utilities.out");

    void printReservoirOutput(bool toFile, string fileName = "Reservoirs.out");

    void printPoliciesOutput(bool toFile, string fileName = "Policies.out");
};


#endif //TRIANGLEMODEL_DATACOLLECTOR_H
