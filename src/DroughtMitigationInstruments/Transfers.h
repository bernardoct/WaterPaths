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
    double average_pipe_capacity = 0;
    map<int, int>* util_id_to_vertex_id = new map<int, int>();
    vector<int> buyers_ids;
    vector<double> allocations;
    vector<double> conveyed_volumes;
    vector<double> buyers_transfer_triggers;
    vector<double> flow_rates_and_allocations;
    Utility *source_utility;
    vector<Utility *> buying_utilities;
    Matrix<double> H, Aeq, A;
    Vector<double> f, beq, b, allocations_aux, lb, ub;

public:

    Transfers(const int id, const int source_utility_id, const double source_treatment_buffer,
                  const vector<int> &buyers_ids, const vector<double> &pipe_transfer_capacities,
                  const vector<double> &buyers_transfer_triggers,
                  const Graph utilities_graph, vector<double> conveyance_costs,
                  vector<int> pipe_owner);

    Transfers(const Transfers &transfers);

    const vector<double> &getAllocations() const;

    const vector<int> &getBuyers_ids() const;

    ~Transfers();

    virtual void applyPolicy(int week) override;

    virtual void addUtility(Utility *utility) override;

    vector<double>
    solve_QP(vector<double> allocation_requests, double available_transfer_volume, double min_transfer_volume);

    vector<double> getConveyed_volumes();
};


#endif //TRIANGLEMODEL_TRANSFERS_H
