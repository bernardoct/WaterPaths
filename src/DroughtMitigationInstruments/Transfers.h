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
    const float source_treatment_buffer;
    float average_pipe_capacity = 0;
    int transfer_water_source_id;
    map<int, int>* util_id_to_vertex_id = new map<int, int>();
    vector<int> buyers_ids;
    vector<float> allocations;
    vector<float> conveyed_volumes;
    vector<float> buyers_transfer_triggers;
    vector<float> flow_rates_and_allocations;
    Utility *source_utility;
    WaterSource *transfer_water_source = nullptr;
    Matrix<float> H, Aeq, A;
    Vector<float> f, beq, b, allocations_aux, lb, ub;

public:

    Transfers(
            const int id, const int source_utility_id,
            int transfer_water_source_id, const float source_treatment_buffer,
            const vector<int> &buyers_ids,
            const vector<float> &pipe_transfer_capacities,
            const vector<float> &buyers_transfer_triggers,
            const Graph utilities_graph, vector<float> conveyance_costs,
            vector<int> pipe_owner);

    Transfers(const Transfers &transfers);

    const vector<float> &getAllocations() const;

    ~Transfers();

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> system_utilities,
                                 vector<WaterSource *> water_sources,
                                 vector<MinEnvironFlowControl *> min_env_flow_controls) override;

    vector<float>
    solve_QP(vector<float> allocation_requests, float available_transfer_volume, float min_transfer_volume,
                 int week);

    void setRealization(unsigned int realization_id) override;

};


#endif //TRIANGLEMODEL_TRANSFERS_H
