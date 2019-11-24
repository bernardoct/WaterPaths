//
// Created by Bernardo on 11/23/2017.
//

#ifndef TRIANGLEMODEL_TRIANGLE_H
#define TRIANGLEMODEL_TRIANGLE_H

#ifdef PARALLEL
#include "../../Borg/borgms.h"
#endif
#include "Base/Problem.h"
#include "../Simulation/Simulation.h"

using namespace std;

class Triangle : public Problem {
private:
    const int n_utilities = 4;

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
    vector<vector<double>> demand_pittsboro;
    vector<vector<double>> demand_chatham;
    vector<double> demand_projection_cary;
    vector<double> demand_projection_durham;
    vector<double> demand_projection_raleigh;
    vector<double> demand_projection_owasa;
    vector<double> demand_projection_pittsboro;
    vector<double> demand_projection_chatham;
    vector<vector<double>> evap_durham;
    vector<vector<double>> evap_falls_lake;
    vector<vector<double>> evap_owasa;
    vector<vector<double>> evap_little_river;
    vector<vector<double>> evap_wheeler_benson;
    vector<vector<double>> evap_jordan_lake;
    vector<vector<double>> demand_to_wastewater_fraction_owasa_raleigh;
    vector<vector<double>> demand_to_wastewater_fraction_durham;
    vector<vector<double>> demand_to_wastewater_fraction_chatham;
    vector<vector<double>> caryDemandClassesFractions;
    vector<vector<double>> durhamDemandClassesFractions;
    vector<vector<double>> raleighDemandClassesFractions;
    vector<vector<double>> owasaDemandClassesFractions;
    vector<vector<double>> pittsboroDemandClassesFractions;
    vector<vector<double>> chathamDemandClassesFractions;
    vector<vector<double>> caryUserClassesWaterPrices;
    vector<vector<double>> durhamUserClassesWaterPrices;
    vector<vector<double>> raleighUserClassesWaterPrices;
    vector<vector<double>> owasaUserClassesWaterPrices;
    vector<vector<double>> pittsboroUserClassesWaterPrices;
    vector<vector<double>> chathamUserClassesWaterPrices;
    vector<vector<double>> owasaPriceSurcharges;

public:
    Triangle(unsigned long n_weeks, int import_export_rof_table);

    ~Triangle();

#ifdef PARALLEL
    void setProblemDefinition(BORG_Problem &problem);
#endif

    int functionEvaluation(double *vars, double *objs, double *consts) override;

    int simulationExceptionHander(const std::exception &e, Simulation *s,
                                  double *objs, const double *vars);

    void readInputData();

};


#endif //TRIANGLEMODEL_TRIANGLE_H
