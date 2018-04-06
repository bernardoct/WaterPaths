//
// Created by Bernardo on 4/4/2018.
//

#ifndef TRIANGLEMODEL_INFRASTRUCTUREMANAGER_H
#define TRIANGLEMODEL_INFRASTRUCTUREMANAGER_H

#include <vector>
#include "../WaterSources/Base/WaterSource.h"

using namespace std;

class InfrastructureManager {

private:
    int id;
    vector<double> infra_construction_triggers;
    vector<int> *priority_draw_water_source;
    vector<int> *non_priority_draw_water_source;
    vector<vector<int>> infra_if_built_remove;
    vector<WaterSource *> *water_sources;
    double infra_discount_rate;
    double bond_term;
    double bond_interest_rate;
    vector<int> rof_infra_construction_order;
    vector<int> demand_infra_construction_order;

    vector<int> infra_built_last_week;
    vector<int> construction_end_date;
    vector<bool> under_construction;
    double infra_net_present_cost = NONE;

public:
    InfrastructureManager(int id, const vector<double> &infra_construction_triggers,
                              const vector<vector<int>> &infra_if_built_remove,
                              double infra_discount_rate, double bond_term,
                              double bond_interest_rate, vector<int> rof_infra_construction_order,
                              vector<int> demand_infra_construction_order);

    InfrastructureManager();

    InfrastructureManager& operator=(const InfrastructureManager& infrastructure_manager);

    InfrastructureManager(InfrastructureManager &infrastructure_manager);

    vector<double> rearrangeInfraRofVector(const vector<double> &infra_construction_triggers,
                                           const vector<int> &rof_infra_construction_order,
                                           const vector<int> &demand_infra_construction_order);

    double setWaterSourceOnline(unsigned int source_id, int week, double &total_storage_capacity,
                         double &total_treatment_capacity, double &total_available_volume,
                         double &total_stored_volume, vector<vector<double>> &debt_payment_streams);

    void waterTreatmentPlantConstructionHandler(unsigned int source_id, double &total_storage_capacity);

    void reservoirExpansionConstructionHandler(unsigned int source_id);

    void sourceRelocationConstructionHandler(unsigned int source_id);

    void removeRelatedSourcesFromQueue(int next_construction);

    int infrastructureConstructionHandler(double long_term_rof, int week,
                                          double past_year_average_demand,
                                          double &total_storage_capacity,
                                          double &total_treatment_capacity,
                                          double &total_available_volume,
                                          double &total_stored_volume,
                                          double &infra_net_present_cost,
                                          vector<vector<double>> &debt_payment_streams);

    void forceInfrastructureConstruction(int week, vector<int> new_infra_triggered);

    void beginConstruction(int week, int infra_id);

    void addWaterSourceToOnlineLists(int source_id, double &total_storage_capacity, double &total_treatment_capacity,
                                     double &total_available_volume, double &total_stored_volume);

    void addWaterSource(WaterSource *water_source);

    void connectWaterSourcesVectors(vector<WaterSource *> &water_sources, vector<int> &priority_draw_water_source,
                                    vector<int> &non_priority_draw_water_source);

    const vector<int> &getRof_infra_construction_order() const;

    const vector<int> &getDemand_infra_construction_order() const;

    const vector<int> &getInfra_built_last_week() const;

    void checkErrorsAddWaterSourceOnline(WaterSource *water_source);
};


#endif //TRIANGLEMODEL_INFRASTRUCTUREMANAGER_H
