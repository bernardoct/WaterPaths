//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include "WaterSources/Reservoir.h"
#include "../Utils/Constants.h"

class Utility {
private:
    double *demand_series;
    double risk_of_failure;
    double total_storage_capacity = 0;
    double total_stored_volume = 0;
    double total_treatment_capacity;
    double demand_multiplier = 1;
    double demand_offset = 0;
    double offset_rate_per_volume = 0;
    double contingency_fund;
    double restricted_demand;
    double unrestricted_demand;
    double infrastructure_net_present_cost = 0;
    bool underConstruction = false;
    int construction_start_date;
    map<int, WaterSource *> water_sources;
    map<int, double> split_demands_among_sources;
    vector<int> infrastructure_construction_order;
    vector<vector<int>> infrastructure_built;

    void setWaterSourceOnline(int source_id);

public:
    const int id;
    const double water_price_per_volume;
    const int number_of_week_demands;
    const string name;
    const double percent_contingency_fund_contribution;
    const double infrastructure_discount_rate;

    Utility(string name, int id, char const *demand_file_name, int number_of_week_demands,
            const double percent_contingency_fund_contribution, const double water_price_per_volume);

    Utility(string name, int id, char const *demand_file_name, int number_of_week_demands,
                const double percent_contingency_fund_contribution, const double water_price_per_volume,
                const vector<int> infrastructure_build_order, double infrastructure_discount_rate);

    Utility(Utility &utility);

    ~Utility();

    Utility &operator=(const Utility &utility);

    bool operator<(const Utility* other);

    bool operator>(const Utility* other);

    void setRisk_of_failure(double risk_of_failure);

    double getRisk_of_failure() const;

    double getReservoirDraw(const int water_source_id);

    void updateTotalStoredVolume();

    const map<int, WaterSource *> &getWaterSource() const;

    void addWaterSource(WaterSource *water_source);

    double getStorageToCapacityRatio() const;

    void splitDemands(int week);

    double getTotal_storage_capacity() const;

    void setDemand_multiplier(double demand_multiplier);

    void setDemand_offset(double demand_offset, double offset_rate_per_volume);

    double getTotal_treatment_capacity() const;

    double getTotal_available_volume() const;

    void updateContingencyFund(int week);

    double getContingency_fund() const;

    double getUnrestrictedDemand() const;

    double getRestrictedDemand() const;

    void beginConstruction(int week);

    void checkBuildInfrastructure(double long_term_rof, int week);

    double getDemand_multiplier() const;

    double getUnrestrictedDemand(int week) const;

    double getInfrastructure_net_present_cost() const;
};


#endif //TRIANGLEMODEL_UTILITY_H
