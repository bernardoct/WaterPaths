//
// Created by David on 2/26/2018.
//

#ifndef DURHAMMODEL_DURHAMMODEL_H
#define DURHAMMODEL_DURHAMMODEL_H

#include "Base/Problem.h"

using namespace std;

class DurhamModel : public Problem {
private:
    vector<vector<Matrix2D<double>>> rof_tables;
    int import_export_rof_tables;
    const int n_utilities = 2;
    double table_gen_storage_multiplier;
    string rof_tables_directory;

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

    void functionEvaluation(double *vars, double *objs, double *consts) override;

    void readInputData();

    void setScenario(int scen) override;

    void setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, string rof_tables_directory);

    void setRofTables(unsigned long n_realizations, int n_weeks, int n_utilities, string rof_tables_directory);
};



#endif //DURHAMMODEL_DURHAMMODEL_H
