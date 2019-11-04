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
    vector<Matrix2D<double>> ut_storage_to_rof_rof_realization;
    vector<int> online_downstream_sources;
    bool *storage_wout_downstream;
    const int n_topo_sources;
    const int use_precomputed_rof_tables;

protected:
    int beginning_tier = 0;
    vector<WaterSource *> realization_water_sources;
    vector<Utility *> realization_utilities;
    vector<Matrix2D<double>> ut_storage_to_rof_table;

    vector<vector<double>> table_storage_shift;
    vector<vector<double>> table_base_storage_shift;
    vector<double> utility_base_storage_capacity;;
    vector<double> utility_base_delta_capacity_table;
    vector<double> current_and_base_storage_capacity_ratio;
    vector<double> current_storage_table_shift;
    vector<double> current_base_storage_table_shift;

public:
    ContinuityModelROF(vector<WaterSource *> water_sources, const Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> utilities,
                       vector<MinEnvFlowControl *> min_env_flow_controls, vector<double>& utilities_rdm,
                       vector<double>& water_sources_rdm, unsigned long total_weeks_simulation,
                       const int use_precomputed_rof_tables, const unsigned long realization_id);

//    ContinuityModelROF(ContinuityModelROF &continuity_model_rof);

    vector<double> calculateShortTermROF(int week, int import_export_rof_tables);

    vector<double> calculateShortTermROFFullCalcs(int week);

    vector<double> calculateShortTermROFTable(int week, vector<Utility *> utilities,
                                              vector<double> utilities_base_storage_capacity,
                                              const vector<Matrix2D<double>> &ut_storage_to_rof_table,
                                              vector<double> current_storage_table_shift,
                                              vector<double> current_base_storage_table_shift);

    vector<vector<double>> calculateLongTermROF(int week);

    void resetUtilitiesAndReservoirs(int rof_type);

    void connectRealizationWaterSources(const vector<WaterSource *> &realization_water_sources);

    void connectRealizationUtilities(const vector<Utility *> &realization_utilities);

    virtual void updateOnlineInfrastructure(int week);

    virtual ~ContinuityModelROF();

    void updateStorageToROFTable(double storage_percent_decrement,
                                 int week_of_the_year,
                                 const double *to_full_toposort);

    vector<Matrix2D<double>> &getUt_storage_to_rof_table();

    void shiftStorages(double *available_volumes_shifted, const double
    *delta_storage);

    void printROFTable(const string &folder);

    void setROFTablesAndShifts(const vector<Matrix2D<double>> &storage_to_rof_table,
                               const vector<vector<double>> &table_storage_shift,
                               const vector<vector<double>> &table_base_storage_shift);

    void tableROFExceptionHandler(double m, int u, int week);

    void setInitialTableTier(int week, const int &utilities, vector<Matrix2D<double>> &vector, int &tier);

    void recordROFStorageTable(vector<Matrix2D<double>> &ut_storage_to_rof_rof_realization,
                               vector<Matrix2D<double>> &ut_storage_to_rof_table, const int &n_utilities, int &week,
                               int &week_of_the_year);

    void calculateEmptyVolumes(vector<WaterSource *> &realization_water_sources, double *to_full);

    void updateJointWTPTreatmentAllocations(const vector<WaterSource *> &non_rof_water_sources);

    void updateUtilityTreatmentAllocations(const vector<Utility *> &non_rof_utilities);
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
