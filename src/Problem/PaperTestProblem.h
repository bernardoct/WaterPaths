//
// Created by dfg42 on 8/28/18.
//

#ifndef TRIANGLEMODEL_PAPERTESTPROBLEM_H
#define TRIANGLEMODEL_PAPERTESTPROBLEM_H

#ifdef  PARALLEL
#include "../../Borg/borgms.h"
#endif
#include "Base/Problem.h"
#include "../Simulation/Simulation.h"

using namespace std;


class PaperTestProblem : public Problem {
private:
    const int n_utilities = 3;

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

    ~PaperTestProblem() override;

#ifdef PARALLEL
    void setProblemDefinition(BORG_Problem &problem);
#endif

    int functionEvaluation(double *vars, double *objs, double *consts) override;

    void readInputData();
};


#endif //TRIANGLEMODEL_PAPERTESTPROBLEM_H
