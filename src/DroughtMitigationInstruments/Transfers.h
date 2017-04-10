//
// Created by bernardo on 2/21/17.
//

#ifndef TRIANGLEMODEL_TRANSFERS_H
#define TRIANGLEMODEL_TRANSFERS_H

#include "DroughtMitigationPolicy.h"
#include "../Utils/QPSolver/QuadProg++.hh"
#include "../Utils/Graph/Graph.h"

class Transfers : public DroughtMitigationPolicy {
private:

    const int source_utility_id;
    const double source_treatment_buffer;
    map<int, int>* util_id_to_vertex_id = new map<int, int>();
    vector<int> buyers_ids;
    vector<double> buyers_transfer_triggers;
    vector<double> flow_rates_and_allocations;
    Utility *source_utility;
    vector<Utility *> buying_utilities;
    Matrix<double> H, Aeq, A;
    Vector<double> f, beq, b, allocations, lb, ub;

public:

    Transfers(const int id, const int source_utility_id, const double source_treatment_buffer,
                  const vector<int> &buyers_ids, const vector<double> &pipe_transfer_capacities,
                  const vector<double> &buyers_transfer_triggers,
                  const vector<vector<double>> *continuity_matrix, vector<double> conveyance_costs,
                  vector<int> pipe_owner);

    Transfers(const Transfers &transfers);

    ~Transfers();

    virtual void applyPolicy(int week) override;

    virtual void addUtility(Utility *utility) override;

    vector<double>
    solve_QP(vector<double> allocation_requests, double available_transfer_volume, double min_transfer_volume);

    vector<double> getTransfers();

    vector<double> getFlowRates();
};


#endif //TRIANGLEMODEL_TRANSFERS_H
