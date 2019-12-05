//
// Created by bernardo on 2/21/17.
//

#ifndef TRIANGLEMODEL_TRANSFERS_H
#define TRIANGLEMODEL_TRANSFERS_H

#include "Base/DroughtMitigationPolicy.h"
#include "../Utils/QPSolver/QuadProg++.h"
#include "../Utils/Graph/Graph.h"

class Transfers : public DroughtMitigationPolicy {
private:

    const int source_utility_id;
    const double source_treatment_buffer;
    double average_pipe_capacity = 0;
    int transfer_water_source_id;
//    map<int, int> util_id_to_vertex_id;
    vector<int> util_id_to_vertex_id;
    vector<int> buyers_ids;
    vector<double> allocations; /// Amount transfered to each utility
    vector<double> conveyed_volumes; /// Flow rate through pipes
    vector<double> buyers_transfer_triggers;
    vector<double> flow_rates_and_allocations;
    Utility *source_utility = nullptr;
    WaterSource *transfer_water_source = nullptr;
    Matrix<double> H, Aeq, A;
    Vector<double> f, beq, b, allocations_aux, lb, ub;

public:

    Transfers(
            const int id, const int source_utility_id,
            int transfer_water_source_id, const double source_treatment_buffer,
            const vector<int> &buyers_ids,
            const vector<double> &pipe_transfer_capacities,
            const vector<double> &buyers_transfer_triggers,
            const Graph utilities_graph, vector<double> conveyance_costs,
            vector<int> pipe_owner);

    Transfers(const Transfers &transfers);

    const vector<double> &getAllocations() const;

    ~Transfers();

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> system_utilities,
                                 vector<WaterSource *> water_sources,
                                 vector<MinEnvFlowControl *> min_env_flow_controls) override;

    vector<double>
    solve_QP(vector<double> allocation_requests, double available_transfer_volume, double min_transfer_volume,
                 int week);

    void setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                        vector<double> &water_sources_rdm, vector<double> &policy_rdm) override;

    const double getSourceTreatmentBuffer() const;

    double getAveragePipeCapacity() const;

    const vector<double> &getBuyersTransferTriggers() const;

    const Matrix<double> &getAeq() const;

    const Vector<double> &getUb() const;
};


#endif //TRIANGLEMODEL_TRANSFERS_H
