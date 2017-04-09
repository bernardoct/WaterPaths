//
// Created by bernardo on 2/21/17.
//

#include "Transfers.h"
#include "../Utils/Utils.h"

/**
 * Allocates transfers by trying to minimize the mean square error between the optimal allocations
 * and allocation requests, and between the available amount of water from source and actual
 * conveyed capacity, which is limited by pipe conveyance capacities.
 *
 * @param id transfer policy ID
 * @param source_utility_id ID of source utility
 * @param source_treatment_buffer treatment capacity to be left unused in source utility
 * @param buyers_ids
 * @param pipe_transfer_capacities
 * @param buyers_transfer_triggers
 */
Transfers::Transfers(const int id, const int source_utility_id, const double source_treatment_buffer,
                     const vector<int> &buyers_ids, const vector<double> &pipe_transfer_capacities,
                     const vector<double> &buyers_transfer_triggers,
                     const vector<vector<double>>* continuity_matrix)
        : DroughtMitigationPolicy(id, TRANSFERS),
          source_utility_id(source_utility_id),
          source_treatment_buffer(source_treatment_buffer),
          buyers_ids(buyers_ids),
          buyers_transfer_triggers(buyers_transfer_triggers) {

    utilities_ids = buyers_ids;
    utilities_ids.push_back(source_utility_id);

    /// Map buyer's IDs to vertexes ID starting from 0 for the sake of building QP matrices.
    for (int i = 0; i < buyers_ids.size(); ++i) {
        util_id_to_vertex_id->insert(pair<int, int>(buyers_ids.at((unsigned long) i), i));
    }

    /// Create QP matrices and vectors.
    /// min f(x) = 1/2 x G x' + g0 x'
    /// s.t.
    /// CE.x = ce0
    /// CI.x >= ci0
    /// lb <= x <= ub
    unsigned int n_flow_rates_Q_source = (unsigned int) pipe_transfer_capacities.size() + 1;
    unsigned int n_allocations = (unsigned int) buyers_ids.size();
    unsigned int n_vars = n_flow_rates_Q_source + n_allocations;

    G.resize(0, n_vars, n_vars);
    for (int i = 0; i < n_vars; ++i)
        if (i < n_flow_rates_Q_source) G[i][i] = 1e-6;
        else G[i][i] = 2;
//    G[n_flow_rates_Q_source - 1][n_flow_rates_Q_source - 1] = 2;

    g0.resize(0, n_vars);

    /// size is number of variables x number of utilities (source + requesting utilities).
    /// Continuity matrix: +1 for pipe entering utility and -1 for leaving.
    CE.resize(0, n_allocations + 1, n_vars);
    cout << endl;
    for (unsigned int i = 0; i < continuity_matrix->size(); ++i) {
        for (unsigned int j = 0; j < continuity_matrix->at(i).size(); ++j) {
            CE[i][j] = continuity_matrix->at(i).at(j);
        }
        cout << endl;
    }

    ce0.resize(0, n_allocations + 1);

    CI.resize(0, 0);

    ci0.resize(0);

    lb.resize(0, n_vars);
    ub.resize(0, n_vars);
    for (int i = 0; i < pipe_transfer_capacities.size(); ++i) {
        lb[i] = -pipe_transfer_capacities[i];
        ub[i] = pipe_transfer_capacities[i];
    }
}

Transfers::Transfers(const Transfers &transfers) : DroughtMitigationPolicy(transfers.id, TRANSFERS),
                                                   source_utility_id(transfers.source_utility_id),
                                                   source_treatment_buffer(transfers.source_treatment_buffer) {

    buyers_ids = transfers.buyers_ids;
    buyers_transfer_triggers = transfers.buyers_transfer_triggers;
    flow_rates_and_allocations = transfers.flow_rates_and_allocations;
//    if (transfers.source_utility)
//        FIXME: VALGRIND COMPLAINS ABOUT THIS LINE ABOVE EVEN IF IT'S NOT SUPPOSED TO BE CALLED.
//        source_utility = new Utility(*transfers.source_utility);

    buying_utilities = Utils::copyUtilityVector(transfers.buying_utilities);

    G = transfers.G;
    CE = transfers.CE;
    CI = transfers.CI;
    g0 = transfers.g0;
    ce0 = transfers.ce0;
    ci0 = transfers.ci0;
    lb = transfers.lb;
    ub = transfers.ub;
    x = transfers.x;
    utilities_ids = transfers.utilities_ids;
    util_id_to_vertex_id = transfers.util_id_to_vertex_id;

}

Transfers::~Transfers() {
    //FIXME: VALGIND COMPLAINS ABOUT DELETING UTILITY HERE.
//    delete source_utility;
}

/**
 * Adds source and buying utility.
 * @param utility Utility to be added.
 */
void Transfers::addUtility(Utility *utility) {
    if (utility->id == source_utility_id)
        source_utility = utility;
    else {
        buying_utilities.push_back(utility);
    }
}

void Transfers::applyPolicy(int week) {

    vector<double> requesting_utilities_rofs(buyers_ids.size(), 0);

    /**
     * Get summation of rofs of utilities needing transfers. This is for splitting the available flow rate and for
     * checking weather any transfers will be needed.
     */
    unsigned int vertex_id;
    double sum_rofs = 0;
    int utilities_requesting_transfers = 0;
    for (auto u : buying_utilities) {
        vertex_id = (unsigned int) util_id_to_vertex_id->at((unsigned int) u->id);
        if (u->getRisk_of_failure() > buyers_transfer_triggers.at(vertex_id)) {
            sum_rofs += u->getRisk_of_failure();
            requesting_utilities_rofs.at(vertex_id) = u->getRisk_of_failure();
        }
    }

    /// Check if transfers will be needed and, if so, perform the transfers.
    if (sum_rofs > 0) {
        cout << week << endl;
        vector<double> transfer_requests(buyers_ids.size(), 0);

        /// Total volume available for transfers in source utility.
        double available_transfer_volume = (source_utility->getTotal_treatment_capacity() - source_treatment_buffer) *
                                           PEAKING_FACTOR - source_utility->getDemand(week);

        /// Minimum volume to be allocated to any utility.
        double min_transfer_volume = available_transfer_volume / (utilities_requesting_transfers + 1);

        /// Split up total volume available among the utilities proportionally to their ROFs.
        for (int i = 0; i < transfer_requests.size(); ++i) {
            transfer_requests[i] = available_transfer_volume * requesting_utilities_rofs[i] / sum_rofs;
        }

        /// Calculate allocations and flow rates through inter-utility connections.
        flow_rates_and_allocations = solve_QP(transfer_requests, available_transfer_volume, min_transfer_volume);

        /// Mitigate demands.
        for (int i = 0; i < buying_utilities.size(); ++i) {
            buying_utilities[i]->setDemand_offset(getFlowRates()[i]);
        }
    }
}

/**
 * Solves Quadratic Programming problem to find optimal allocations given transfer requests and constraints in
 * conveyance capacities.
 * @param allocation_requests allocations requested by each utility (0 for utilities not requesting transfers).
 * @param available_transfer_volume Total volume available for transfer from source utility.
 * @param min_transfer_volume minimum transfer to be made to each utility.
 * @return
 */
vector<double> Transfers::solve_QP(vector<double> allocation_requests, double available_transfer_volume,
                                   double min_transfer_volume) {

    unsigned int n_allocations = (unsigned int) allocation_requests.size();
    unsigned int n_flow_rates_Q_source = g0.size() - n_allocations;
    unsigned int n_vars = g0.size();

    Matrix<double> G = this->G;

    /// Set g0 vector to allocated to 2 * target_allocation.
    for (int i = 0; i < allocation_requests.size(); ++i) {
        g0[i + n_flow_rates_Q_source] = -2 * allocation_requests[i];
    }
//    g0[n_flow_rates_Q_source - 1] = -2 * available_transfer_volume;

    /// Set allocation bounds to 0 if no allocation is requested, and to
    /// minimum allocation and available transfer volume.
    for (int i = 0; i < n_allocations; ++i) {
        if (allocation_requests[i] == NONE) {
//            lb[i + n_flow_rates_Q_source] = NONE;
            ub[i + n_flow_rates_Q_source] = NONE;
        } else {
//            lb[i + n_flow_rates_Q_source] = min_transfer_volume;
            ub[i + n_flow_rates_Q_source] = available_transfer_volume;
        }
    }

    /// Set the last utility (source) to total amount of water provided.
    ub[n_flow_rates_Q_source - 1] = available_transfer_volume;

    /// Prevent minimum allocation from being more than capacity of pipes connected to utility.
//    double max_flow;
//    for (int i = 1; i < n_allocations + 1; ++i) { // skip source
//        max_flow = 0;
//        for (int j = 0; j < n_flow_rates_Q_source; ++j) {
//            max_flow += abs(CE[i][j] * ub[j]);
//        }
//        if (lb[n_flow_rates_Q_source + i - 1] > max_flow)
//            lb[n_flow_rates_Q_source + i - 1] = max_flow;
//    }


    print_matrix("G", G);
    print_vector("g0", g0);
    print_matrix("CE", CE);
    print_vector("ce0", ce0);
    print_vector("lb", lb);
    print_vector("ub", ub);

    Vector<double> x(n_vars);

    /// Run quadratic programming solver.
    solve_quadprog_matlab_syntax(G, g0, CE, ce0, CI, ci0, lb, ub, x);
    print_vector("x", x);

    /// Convert Vector x into a vector.
    for (int i = 0; i < x.size(); ++i) {
        flow_rates_and_allocations.push_back(x[i]);
    }
    cout << endl;

    return flow_rates_and_allocations;
}

vector<double> Transfers::getTransfers() {
    vector<double> transfers = *(new vector<double>(flow_rates_and_allocations.end() - buyers_ids.size(),
                                                     flow_rates_and_allocations.end()));
    return transfers;
}

vector<double> Transfers::getFlowRates() {
    vector<double> transfers = *(new vector<double>(flow_rates_and_allocations.begin(),
                                                    flow_rates_and_allocations.end() - buyers_ids.size()));
    return transfers;
}