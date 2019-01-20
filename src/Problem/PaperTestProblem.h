//
// Created by dfg42 on 8/28/18.
//

#ifndef TRIANGLEMODEL_PAPERTESTPROBLEM_H
#define TRIANGLEMODEL_PAPERTESTPROBLEM_H

#include "Base/Problem.h"
#include "../Simulation/Simulation.h"

using namespace std;


class PaperTestProblem : public Problem {
private:
    vector<vector<Matrix2D<double>>> rof_tables;
    int import_export_rof_tables;
    const int n_utilities = 3;
    double table_gen_storage_multiplier;
    string rof_tables_directory;

    vector<vector<double>> streamflows_durham;
    vector<vector<double>> streamflows_flat;
    vector<vector<double>> streamflows_llr;
    vector<vector<double>> streamflows_swift;
    vector<vector<double>> streamflows_phils;
    vector<vector<double>> streamflows_cane;
    vector<vector<double>> streamflows_morgan;
    vector<vector<double>> streamflows_haw;
    vector<vector<double>> streamflows_lillington;
    vector<vector<double>> evap_durham;
    vector<vector<double>> evap_falls_lake;
    vector<vector<double>> evap_owasa;
    vector<vector<double>> evap_little_river;
    vector<vector<double>> evap_wheeler_benson;
    vector<vector<double>> evap_jordan_lake;
    vector<vector<double>> demand_watertown;
    vector<vector<double>> demand_dryville;
    vector<vector<double>> demand_fallsland;
    vector<vector<double>> demand_to_wastewater_fraction_fallsland;
    vector<vector<double>> demand_to_wastewater_fraction_dryville;
    vector<vector<double>> watertownDemandClassesFractions;
    vector<vector<double>> dryvilleDemandClassesFractions;
    vector<vector<double>> fallslandDemandClassesFractions;
    vector<vector<double>> watertownUserClassesWaterPrices;
    vector<vector<double>> dryvilleUserClassesWaterPrices;
    vector<vector<double>> fallslandUserClassesWaterPrices;

public:
    PaperTestProblem(unsigned long n_weeks, int import_export_rof_table);

    ~PaperTestProblem();

    int functionEvaluation(double *vars, double *objs, double *consts) override;


    int simulationExceptionHander(const std::exception &e, Simulation *s,
                                  double *objs, const double *vars);

    void setRofTables(unsigned long n_realizations, int n_utilities, string rof_tables_directory);

    void setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, string rof_tables_directory);

    void readInputData();
};


#endif //TRIANGLEMODEL_PAPERTESTPROBLEM_H
