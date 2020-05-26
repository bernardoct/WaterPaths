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
    vector<double> infra_construction_triggers;
    vector<int> *priority_draw_water_source;
    vector<int> *non_priority_draw_water_source;
    vector<vector<int>> infra_if_built_remove;
    vector<WaterSource *> *water_sources;
    int id;
    double infra_discount_rate;
    string name;
    vector<int> rof_infra_construction_order;
    vector<int> demand_infra_construction_order;

    vector<int> infra_built_last_week;
    vector<int> triggered_queue;
    vector<int> construction_end_date;
    vector<bool> under_construction;

public:
    InfrastructureManager(string name, int id,
                          const vector<double> &infra_construction_triggers,
                          const vector<vector<int>> &infra_if_built_remove,
                          double infra_discount_rate,
                          vector<int> rof_infra_construction_order,
                          vector<int> demand_infra_construction_order);

    InfrastructureManager();

    InfrastructureManager &
    operator=(const InfrastructureManager &infrastructure_manager);

    InfrastructureManager(InfrastructureManager &infrastructure_manager);

    vector<double>
    rearrangeInfraRofVector(const vector<double> &infra_construction_triggers,
                            const vector<int> &rof_infra_construction_order,
                            const vector<int> &demand_infra_construction_order);

    void setWaterSourceOnline(unsigned int source_id, int week,
                              vector<double> &utility_owned_wtp_capacities,
                              vector<int> &water_source_to_wtp,
                              double &total_storage_capacity,
                              double &total_available_volume,
                              double &total_stored_volume);

    void waterTreatmentPlantConstructionHandler(unsigned int source_id,
                                                double &total_storage_capacity,
                                                vector<double> &utility_owned_wtp_capacities,
                                                vector<int> &water_source_to_wtp);

    void reservoirExpansionConstructionHandler(unsigned int source_id);

    void sourceRelocationConstructionHandler(unsigned int source_id);

    void removeRelatedSourcesFromQueue(int next_construction);

    int infrastructureConstructionHandler(
            double long_term_rof, int week, double past_year_average_demand,
            vector<double> &utility_owned_wtp_capacities,
            vector<int> &water_source_to_wtp,
            double &total_storage_capacity,
            double &total_available_volume, double &total_stored_volume);

    void
    forceInfrastructureConstruction(int week, vector<int> new_infra_triggered);

    void beginConstruction(int week, int infra_id);

    void
    addWaterSourceToOnlineLists(int source_id, double &total_storage_capacity,
                                double &total_available_volume,
                                double &total_stored_volume);

    void addWaterSource(WaterSource *water_source);

    void
    connectWaterSourcesVectorsToUtilitys(vector<WaterSource *> &water_sources,
                                          vector<int> &priority_draw_water_source,
                                          vector<int> &non_priority_draw_water_source);

    int getIdOfNewTriggeredInfra(double trigger_var, int week,
                                 bool under_construction_any,
                                 const vector<int> &construction_order);

    static void
    restructureVectorOfRelatedInfrastructure(vector<vector<int>> &vec,
                                             const vector<vector<int>> &vec_original,
                                             int size);

    const vector<int> &getRof_infra_construction_order() const;

    const vector<int> &getDemand_infra_construction_order() const;

    const vector<int> &getInfra_built_last_week() const;

    const vector<bool> &getUnder_construction() const;

    const vector<double> &getInfraConstructionTriggers() const;

    int getId() const;

    double getInfraDiscountRate() const;
};


#endif //TRIANGLEMODEL_INFRASTRUCTUREMANAGER_H
