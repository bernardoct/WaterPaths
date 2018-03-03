//
// Created by David on 2/26/2018.
//

#ifndef DURHAMMODEL_DURHAMMODEL_H
#define DURHAMMODEL_DURHAMMODEL_H

#include "Base/Problem.h"

using namespace std;

class DurhamModel : public Problem {
private:
    vector<Matrix3D<double>> rof_tables;
    vector<vector<double>> infra_table_shift;
    int import_export_rof_tables;
    const int n_utilities = 2;
    double table_gen_storage_multiplier;
    int scenario;

    vector<vector<double>> streamflows_durham;
    vector<vector<double>> streamflows_haw;
    vector<vector<double>> streamflows_lillington;
    vector<vector<double>> demand_cary;
    vector<vector<double>> demand_durham;
    vector<vector<double>> evap_durham;
    vector<vector<double>> evap_owasa;
    vector<vector<double>> evap_jordan_lake;
    vector<vector<double>> demand_to_wastewater_fraction_durham;
    vector<vector<double>> caryDemandClassesFractions;
    vector<vector<double>> durhamDemandClassesFractions;
    vector<vector<double>> caryUserClassesWaterPrices;
    vector<vector<double>> durhamUserClassesWaterPrices;

public:
    DurhamModel(unsigned long n_weeks, int import_export_rof_table);

    ~DurhamModel();

    void functionEvaluation(const double *vars, double *objs, double *consts) override;

    void setRofTables(unsigned long n_realizations, int n_weeks, int n_utilities);

    void setImport_export_rof_tables(int import_export_rof_tables, int n_weeks);

    void readInputData();

    void setScenario(int scen);

    void delete_catchment_vector(vector<Catchment *> vec_catchment);
};



#endif //DURHAMMODEL_DURHAMMODEL_H
