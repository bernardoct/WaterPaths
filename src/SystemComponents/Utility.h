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
    double total_storage_capacity;
    double total_stored_volume;
    double total_treatment_capacity;
    double contingency_fund;
    double water_price_per_volume;
public:
    double getWater_price_per_volume() const;

private:
    map<int, WaterSource *> water_sources;
    map<int, double> split_demands_among_sources;

    void setWaterSourceOnline(int source_id);

public:
    const int id;

    double getRisk_of_failure() const;

    void setRisk_of_failure(double risk_of_failure);

    const int number_of_week_demands;
    const string name;

    Utility(string const name, int id, char const *demand_file_name, int number_of_week_demands);

    Utility(Utility &utility);

    ~Utility();

    Utility &operator=(const Utility &utility);

    bool operator<(const Utility *utility);

    double getReservoirDraw(const int water_source_id);

    void drawFromContingencyFund(double amount);

    void addToContingencyFund(double amount);

    void updateTotalStoredVolume();

    const map<int, WaterSource *> &getWaterSource() const;

    void addWaterSource(WaterSource *water_source);

    double getStorageToCapacityRatio() const;

    void splitDemands(int week);

    double getTotal_storage_capacity() const;

    void setDemand(int week, double weekly_demand);

    double getDemand(const int week);

    double getTotal_treatment_capacity() const;

    double getTotal_available_volume() const;
};


#endif //TRIANGLEMODEL_UTILITY_H