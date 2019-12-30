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
#include "Base/HardCodedProblem.h"

using namespace std;


class PaperTestProblem : public HardCodedProblem {
private:
    const int n_utilities = 3;

    vector<vector<double>> streamflows_durham;
    vector<vector<double>> streamflows_clayton;
    vector<vector<double>> streamflows_flat;
    vector<vector<double>> streamflows_llr;
    vector<vector<double>> streamflows_swift;
    vector<vector<double>> streamflows_phils;
    vector<vector<double>> streamflows_cane;
    vector<vector<double>> streamflows_morgan;
    vector<vector<double>> streamflows_crabtree;
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

    void readInputData() override;

public:
    PaperTestProblem(unsigned long n_weeks, int n_realizations,
                     int import_export_rof_table,
                     string system_io, string &rof_tables_directory, int seed,
                     unsigned long n_threads,
                     string bootstrap_file,
                     string &utilities_rdm_file,
                     string &policies_rdm_file,
                     string &water_sources_rdm_file,
                     int n_sets, int n_bs_samples,
                     string &solutions_file,
                     vector<int> &solutions_to_run_range, bool plotting,
                     bool print_obj_row);

    ~PaperTestProblem() override;

#ifdef PARALLEL
    void setProblemDefinition(BORG_Problem &problem) override;
#endif

    int functionEvaluation(double *vars, double *objs, double *consts) override;

};


#endif //TRIANGLEMODEL_PAPERTESTPROBLEM_H
