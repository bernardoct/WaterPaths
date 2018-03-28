//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "Base/ContinuityModel.h"
#include "../Utils/Matrices.h"


class ContinuityModelROF : public ContinuityModel {
private:
//    Matrix3D<double> storage_to_rof_table;
//    Matrix3D<double> storage_to_rof_rof_realization;
    vector<Matrix2D<double>> ut_storage_to_rof_table;
    vector<Matrix2D<double>> ut_storage_to_rof_rof_realization;
    vector<int> online_downstream_sources;
    bool *storage_wout_downstream;
    const int n_topo_sources;
    const bool use_precomputed_rof_tables;

protected:
    int beginning_tier = 0;
    vector<WaterSource *> realization_water_sources;
    vector<Utility *> realization_utilities;

    vector<vector<double>> table_storage_shift;
    vector<double> utility_base_storage_capacity;;
    vector<double> utility_base_delta_capacity_table;
    vector<double> current_and_base_storage_capacity_ratio;
    vector<double> current_storage_table_shift;

public:
    ContinuityModelROF(vector<WaterSource *> water_sources, const Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> utilities,
                       vector<MinEnvFlowControl *> min_env_flow_controls, vector<double>& utilities_rdm,
                       vector<double>& water_sources_rdm, unsigned long total_weeks_simulation,
                       const bool use_precomputed_rof_tables, const unsigned int realization_id);

    ContinuityModelROF(ContinuityModelROF &continuity_model_rof);

    vector<double> calculateShortTermROF(int week);

    vector<double> calculateLongTermROF(int week);

    void resetUtilitiesAndReservoirs(int rof_type);

    void connectRealizationWaterSources(const vector<WaterSource *> &realization_water_sources);

    void connectRealizationUtilities(const vector<Utility *> &realization_utilities);

    void updateOnlineInfrastructure(int week);

    virtual ~ContinuityModelROF();

    void updateStorageToROFTable(double storage_percent_decrement,
                                 int week_of_the_year,
                                 const double *to_full_toposort);

    const vector<Matrix2D<double>> &getUt_storage_to_rof_table() const;

    void shiftStorages(double *available_volumes_shifted, const double
    *delta_storage);

    void printROFTable(const string &folder);

    void setROFTablesAndShifts(const vector<Matrix2D<double>> &storage_to_rof_table,
                               const vector<vector<double>> &table_storage_shift);

    vector<double> calculateShortTermROFTable(int week);

    void tableROFExceptionHandler(double m, int u, int week);


};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
