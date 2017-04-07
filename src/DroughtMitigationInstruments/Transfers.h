//
// Created by bernardo on 2/21/17.
//

#ifndef TRIANGLEMODEL_TRANSFERS_H
#define TRIANGLEMODEL_TRANSFERS_H

#include "DroughtMitigationPolicy.h"
#include "../Utils/QPSolver/QuadProg++.hh"
#include "../Utils/Graph.h"

class Transfers : public DroughtMitigationPolicy {
private:

    const int source_utility_id;
    const double source_treatment_buffer;
    vector<int> buyers_ids;
    vector<double> buyers_transfer_triggers;
    vector<double> flow_rates_and_allocations;
    Utility *source_utility;
    map<int, Utility *> buying_utilities;
    Matrix<double> G, CE, CI;
    Vector<double> g0, ce0, ci0, x;

public:

    Transfers(const int id, const int source_utility_id, const double source_treatment_buffer,
              const vector<int> &buyers_ids, const vector<double> &buyers_transfer_capacities,
              const vector<double> &buyers_transfer_triggers, vector<vector<double>> continuity_matrix);

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
