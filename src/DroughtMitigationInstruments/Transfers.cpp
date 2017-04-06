//
// Created by bernardo on 2/21/17.
//

#include "Transfers.h"
#include "../Utils/Graph.h"

/**
 *
 * @param id
 * @param source_utility_id
 * @param source_treatment_buffer
 * @param buyers_ids
 * @param buyers_transfer_capacities
 * @param buyers_transfer_triggers
 */
Transfers::Transfers(const int id, const int source_utility_id, const double source_treatment_buffer,
                     const vector<int> &buyers_ids, const vector<double> &buyers_transfer_capacities,
                     const vector<double> &buyers_transfer_triggers, vector<vector<double>> continuity_matrix)
        : DroughtMitigationPolicy(id),
          source_utility_id(source_utility_id),
          source_treatment_buffer(source_treatment_buffer),
          buyers_ids(buyers_ids) {

    /// Create QP matrices and vectors.
    /// min f(x) = 1/2 x G x' + g0 x'
    /// s.t.
    /// CE.x = ce0
    /// CI.x >= ci0
    unsigned int n_flow_rates = (unsigned int) buyers_transfer_capacities.size();
    unsigned int n_allocations = (unsigned int) buyers_ids.size();
    unsigned int n_vars = n_flow_rates + n_allocations;

    G.resize(0, n_vars, n_vars);
    for (int i = 0; i < n_vars; ++i)
        if (i < n_flow_rates) G[i][i] = 1e-6;
        else G[i][i] = 2;

    g0.resize(0, n_vars);

    /// size is number of variables x number of utilities (source + requesting utilities).
    /// Continuity matrix: +1 for pipe entering utility and -1 for leaving.
    CE.resize(0, n_vars, n_allocations + 1);
    for (unsigned int i = 0; i < continuity_matrix.size(); ++i) {
        for (unsigned int j = 0; j < continuity_matrix[i].size(); ++j) {
            CE[i][j] = continuity_matrix.at(i).at(j);
        }
    }

    ce0.resize(0, n_allocations + 1);

    /// +1 for >= constraints and -1 for <= to account for bi-directional pipes.
    CI.resize(0, n_vars, 2 * n_vars);
    for (int i = 0; i < n_vars; ++i) {
        CI[i][i] = 1;
        CI[i][i + n_vars] = -1;
    }

    ci0.resize(0, n_vars * 2);
    for (unsigned int i = 0; i < buyers_transfer_capacities.size(); ++i) {
        ci0[i] = buyers_transfer_capacities.at(i);
        ci0[i + n_vars] = buyers_transfer_capacities.at(i);
    }
}

/**
 * Adds source and buying utility. The highest utility ID is used to create vectors of appropriate length in other
 * methods.
 * @param utility Utility to be added.
 */
void Transfers::addUtility(Utility *utility) {
    if (utility->id == source_utility_id)
        source_utility = utility;
    else {
        buying_utilities.insert(pair<int, Utility *>(utility->id, utility));
    }
}

void Transfers::applyPolicy(int week) {

    vector<double> requesting_utilities_rofs(buyers_ids.size(), 0);

    /**
     * Get summation of rofs of utilities needing transfers. This is for splitting the available flow rate and for
     * checking weather any transfers will be needed.
     */
    int id;
    double sum_rofs = 0;
    int utilities_requesting_transfers = 0;
    for (auto u : buying_utilities) {
        id = u.second->id;
        if (u.second->getRisk_of_failure() > buyers_transfer_triggers[id]) {
            sum_rofs += u.second->getRisk_of_failure();
            requesting_utilities_rofs[id] = u.second->getRisk_of_failure();
            utilities_requesting_transfers++;
        }
    }

    /// Check if transfers will be needed and, if so, perform the transfers.
    if (sum_rofs > 0) {
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
        vector<double> flow_rates_and_allocations = solve_QP(transfer_requests,
                                                             available_transfer_volume, min_transfer_volume);

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
    unsigned int n_flow_rates = g0.size() - n_allocations;
    unsigned int n_vars = g0.size();

    /// Set g0 vector to allocated to 2 * target_allocation.
    for (int i = 0; i < allocation_requests.size(); ++i) {
        g0[i + n_flow_rates] = -2 * allocation_requests[i];
    }

    /// Set the last utility (source) to total amount of water provided.
    ce0[ce0.size() - 1] = -available_transfer_volume;

    /// Set allocation bounds to 0 if no allocation is requested, and to
    /// minimum allocation and available transfer volume.
    for (int i = 0; i < allocation_requests.size(); ++i) {
        if (allocation_requests[i] == 0) {
            ci0[i + n_flow_rates] = 0;
            ci0[i + n_flow_rates + n_vars] = 0;
        }
        else {
            ci0[i + n_flow_rates] = -min_transfer_volume;
            ci0[i + n_flow_rates + n_vars] = available_transfer_volume;
        }
    }
/*    cout << "G" << endl;
    for (int i = 0; i < G.nrows(); ++i) {
        for (int j = 0; j < G.ncols(); ++j)
            cout << G[i][j] << " ";
        cout << endl;
    }

    cout << "g0" << endl;
    for (int i = 0; i < g0.size(); ++i) {
        cout << g0[i] << " ";
    }
    cout << endl;

    cout << "CE" << endl;
    for (int i = 0; i < CE.nrows(); ++i) {
        for (int j = 0; j < CE.ncols(); ++j)
            cout << CE[i][j] << " ";
        cout << endl;
    }

    cout << "ce0" << endl;
    for (int i = 0; i < ce0.size(); ++i) {
        cout << ce0[i] << " ";
    }
    cout << endl;

    cout << "CI" << endl;
    for (int i = 0; i < CI.nrows(); ++i) {
        for (int j = 0; j < CI.ncols(); ++j)
            cout << CI[i][j] << " ";
        cout << endl;
    }

    cout << "ci0" << endl;
    for (int i = 0; i < ci0.size(); ++i) {
        cout << ci0[i] << " ";
    }
    cout << endl; */

    Vector<double> x(n_vars);

    /// Run quadratic programming solver.
    std::cout << "f: " << solve_quadprog(G, g0, CE, ce0, CI, ci0, x) << std::endl;

    for (int i = 0; i < x.size(); ++i) {
        flow_rates_and_allocations.push_back(x[i]);
    }


    return flow_rates_and_allocations;
}

vector<double> Transfers::getTransfers() {
    return *(new vector<double>(&flow_rates_and_allocations[flow_rates_and_allocations.size() - buyers_ids.size()],
                                &flow_rates_and_allocations[flow_rates_and_allocations.size() - 1]));
}