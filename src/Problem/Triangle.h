//
// Created by Bernardo on 11/23/2017.
//

#ifndef TRIANGLEMODEL_TRIANGLE_H
#define TRIANGLEMODEL_TRIANGLE_H

#include "Base/Problem.h"
#include "../Simulation/Simulation.h"

using namespace std;

class Triangle : public Problem {
private:
    vector<vector<Matrix2D<double>>> rof_tables;
    int import_export_rof_tables;
    const int n_utilities = 4;
    double table_gen_storage_multiplier;
    string rof_tables_directory;

    vector<vector<double>> streamflows_durham;
    vector<vector<double>> streamflows_flat;
    vector<vector<double>> streamflows_swift;
    vector<vector<double>> streamflows_llr;
    vector<vector<double>> streamflows_crabtree;
    vector<vector<double>> streamflows_phils;
    vector<vector<double>> streamflows_cane;
    vector<vector<double>> streamflows_morgan;
    vector<vector<double>> streamflows_haw;
    vector<vector<double>> streamflows_clayton;
    vector<vector<double>> streamflows_lillington;
    vector<vector<double>> demand_cary;
    vector<vector<double>> demand_durham;
    vector<vector<double>> demand_raleigh;
    vector<vector<double>> demand_owasa;
    vector<vector<double>> evap_durham;
    vector<vector<double>> evap_falls_lake;
    vector<vector<double>> evap_owasa;
    vector<vector<double>> evap_little_river;
    vector<vector<double>> evap_wheeler_benson;
    vector<vector<double>> evap_jordan_lake;
    vector<vector<double>> demand_to_wastewater_fraction_owasa_raleigh;
    vector<vector<double>> demand_to_wastewater_fraction_durham;
    vector<vector<double>> caryDemandClassesFractions;
    vector<vector<double>> durhamDemandClassesFractions;
    vector<vector<double>> raleighDemandClassesFractions;
    vector<vector<double>> owasaDemandClassesFractions;
    vector<vector<double>> caryUserClassesWaterPrices;
    vector<vector<double>> durhamUserClassesWaterPrices;
    vector<vector<double>> raleighUserClassesWaterPrices;
    vector<vector<double>> owasaUserClassesWaterPrices;
    vector<vector<double>> owasaPriceSurcharges;

public:
    Triangle(unsigned long n_weeks, int import_export_rof_table);

    ~Triangle();

    int functionEvaluation(double *vars, double *objs, double *consts) override;

    int simulationExceptionHander(const std::exception &e, Simulation *s,
                                       double *objs, const double *vars);

    void setRofTables(unsigned long n_realizations, int n_utilities, string rof_tables_directory);

    void setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, string rof_tables_directory);

    void readInputData();

};


#endif //TRIANGLEMODEL_TRIANGLE_H
