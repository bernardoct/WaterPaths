//
// Created by bernardo on 2/21/17.
//

#include <numeric>
#include "Transfers.h"
#include "../Utils/Utils.h"


/**
 * Allocates transfers by trying to minimize the mean square error between the
 * optimal allocations and allocation requests, and between the available
 * amount of water from source and actual conveyed capacity, which is limited
 * by pipe conveyance capacities.
 *
 * @param id transfer policy ID.
 * @param source_utility_id ID of source utility.
 * @param source_treatment_buffer treatment capacity to be left unused in source
 * utility.
 * @param buyers_ids
 * @param pipe_transfer_capacities
 * @param buyers_transfer_triggers
 * @param conveyance_costs amount charged by pipe owning utility to convey water
 * to another utility.
 * @param pipe_owner id of utility owning each pipe.
 *
 * @todo add the possibility of two or more sources.
 * @todo add the possibility of having a pipe belong to a utility not in the
 * transfer agreement. This can be accomplished now by adding utility to
 * transfer agreement and set its risk of failure to 1.1 or more.
 * @todo calculation and charge of wheeling fees.
 */
Transfers::Transfers(
        const int id, const int source_utility_id,
        int transfer_water_source_id, const double source_treatment_buffer,
        const vector<int> &buyers_ids,
        const vector<double> &pipe_transfer_capacities,
        const vector<double> &buyers_transfer_triggers,
        const Graph utilities_graph, vector<double> conveyance_costs,
        vector<int> pipe_owner)
        : DroughtMitigationPolicy(id, TRANSFERS),
          source_utility_id(source_utility_id),
          source_treatment_buffer(source_treatment_buffer),
          buyers_ids(buyers_ids),
          buyers_transfer_triggers(buyers_transfer_triggers),
          transfer_water_source_id(transfer_water_source_id) {

    for (int i : buyers_ids)
        if (i == source_utility_id)
            throw std::invalid_argument("Source utility cannot be a buyer "
                                                "utility as well.");

    utilities_ids = buyers_ids;
    utilities_ids.push_back(source_utility_id);
    allocations = vector<double>(utilities_ids.size(), 0);
    for (const double &d : pipe_transfer_capacities)
        average_pipe_capacity += d;

    vector<vector<double>> continuity_matrix =
            utilities_graph.getContinuityMatrix();
    continuity_matrix[source_utility_id][
            continuity_matrix.size() + source_utility_id] = 1;

    /// Map buyer's IDs to vertexes ID starting from 0 for the sake of
    /// determining payments.
    for (int i = 0; i < buyers_ids.size(); ++i) {
        util_id_to_vertex_id->insert(
                pair<int, int>(buyers_ids.at((unsigned long) i),
                               i));
    }

    /// Create QP matrices and vectors.
    /// min f(x) = 1/2 x G x' + g0 x'
    /// s.t.
    /// CE.x = ce0
    /// CI.x >= ci0
    /// lb <= x <= ub
    unsigned int n_flow_rates_Q_source =
            (unsigned int) pipe_transfer_capacities.size() + 1;
    auto n_allocations = (unsigned int) buyers_ids.size();
    unsigned int n_vars = n_flow_rates_Q_source + n_allocations;

    H.resize(0, n_vars, n_vars);
    for (int i = 0; i < n_vars; ++i)
        if (i < n_flow_rates_Q_source) H[i][i] = 1e-6;
        else H[i][i] = 2;

    /// Create g0 vector of 0's to be filled when policy is applied.
    f.resize(0, n_vars);

    /// size is number of variables x number of utilities (source + requesting
    /// utilities). Continuity matrix: +1 for flow entering utility and -1
    /// for leaving.
    Aeq.resize(0, n_allocations + 1, n_vars);
    for (unsigned int i = 0; i < continuity_matrix.size(); ++i) {
        for (unsigned int j = 0; j < continuity_matrix.at(i).size(); ++j) {
            Aeq[i][j] = continuity_matrix.at(i).at(j);
        }
    }

    /// Create ce0 vector of 0 so that mass balance is enforced on the nodes
    /// (utilities).
    beq.resize(0, n_allocations + 1);

    /// Create empty CI and ci0. They'll be re-dimensioned in the QP solver.
    A.resize(0, 0);

    b.resize(0);

    /// Fill in pipe capacities part of lower and upper bound vectors.
    lb.resize(0, n_vars);
    ub.resize(0, n_vars);
    for (int i = 0; i < pipe_transfer_capacities.size(); ++i) {
        lb[i] = -pipe_transfer_capacities[i];
        ub[i] = pipe_transfer_capacities[i];
    }
}

/**
 * Copy constructor
 * @param transfers
 */
Transfers::Transfers(const Transfers &transfers) :
        DroughtMitigationPolicy(transfers.id,
                                TRANSFERS),
        source_utility_id(transfers.source_utility_id),
        source_treatment_buffer(transfers.source_treatment_buffer) {

//    if (transfers.source_utility)
//        FIXME: VALGRIND COMPLAINS ABOUT THIS LINE ABOVE EVEN IF IT'S NOT SUPPOSED TO BE CALLED.
//        source_utility = new Utility(*transfers.source_utility);
    buyers_ids = transfers.buyers_ids;
    buyers_transfer_triggers = transfers.buyers_transfer_triggers;
    flow_rates_and_allocations = transfers.flow_rates_and_allocations;
    transfer_water_source_id = transfers.transfer_water_source_id;
    H = transfers.H;
    Aeq = transfers.Aeq;
    A = transfers.A;
    f = transfers.f;
    beq = transfers.beq;
    b = transfers.b;
    lb = transfers.lb;
    ub = transfers.ub;
    allocations_aux = transfers.allocations_aux;
    utilities_ids = transfers.utilities_ids;
    util_id_to_vertex_id = transfers.util_id_to_vertex_id;
    allocations = transfers.allocations;
    conveyed_volumes = transfers.conveyed_volumes;
    average_pipe_capacity = transfers.average_pipe_capacity;
}

/**
 * Destructor
 */
Transfers::~Transfers() {
    //FIXME: I'M GETTING SEGMENTATION FAULT HERE.
//    if (source_utility)
//       delete source_utility;
}

/**
 * Adds source and buying utility.
 * @param system_utilities Utility to be added.
 */
void Transfers::addSystemComponents(
        vector<Utility *> system_utilities,
        vector<WaterSource *> water_sources) {

    if (!realization_utilities.empty())
        throw std::invalid_argument("Utilities were already assigned to "
                                            "transfer policy.");

    //FIXME: RIGHT NOW TRANSFERS CAN ONLY HAVE ONE SOURCE. THIS NEEDS TO BE EXPANDED.
    for (Utility *u : system_utilities)
        if (u->id == source_utility_id)
            source_utility = u; // source of transfers
        else {
            realization_utilities.push_back(u); //
        }

    if (transfer_water_source != nullptr)
        __throw_invalid_argument("Water sources already assigned to transfer "
                                         "policy.");
    else
        transfer_water_source = water_sources[transfer_water_source_id];
}

void Transfers::applyPolicy(int week) {

    vector<double> requesting_utilities_rofs(buyers_ids.size(), 0);
    std::fill(allocations.begin(), allocations.end(), 0);

    int n_vars = f.size();
    int n_allocations = (int) buyers_ids.size();
    int n_utilities = n_allocations + 1;
    int n_pipes = n_vars - n_utilities;

    /**
     * Get summation of rofs of utilities needing transfers. This is for
     * splitting the available flow rate and for
     * checking weather any transfers will be needed.
     */
    unsigned int vertex_id; // position of utility id in the buyers_transfer_triggers vector.
    double sum_rofs = 0;
    int utilities_requesting_transfers = 0;
    for (auto u : realization_utilities) {
        vertex_id = (unsigned int) util_id_to_vertex_id->at((unsigned int) u->id);
        if (u->getRisk_of_failure() > buyers_transfer_triggers.at(vertex_id)) {
            sum_rofs += u->getRisk_of_failure();
            requesting_utilities_rofs.at(vertex_id) = u->getRisk_of_failure();
            utilities_requesting_transfers++;
        }
    }

    /// Check if transfers will be needed and, if so, perform the transfers.
    if (sum_rofs > 0) {
        vector<double> transfer_requests((unsigned long) n_allocations, 0);

        /// Total volume available for transfers in source utility.
        double available_transfer_volume =
                (source_utility->getTotal_treatment_capacity()
                 - source_treatment_buffer) * PEAKING_FACTOR
                - source_utility->getUnrestrictedDemand();

        if (available_transfer_volume > 0) {

            /// Minimum volume to be allocated to any utility.
            double min_transfer_volume = available_transfer_volume
                                         / (utilities_requesting_transfers + 1);

            /// Split up total volume available among the utilities
            /// proportionally to their ROFs. Requests are scaled with the
            /// summation of the pipe capacities so to scale them down in case
            /// there is much more water available than the pipes can convey,
            /// so to avaid the utility with the highest request from getting
            /// all the water. Other scaling factors can be used as well.
            //FIXME: FIGURE OUT WHAT SCALING FACTOR THAT BEST APPROXIMATES CURRENT NC TRIANGLE RULES IN HB'S MODEL.
            for (int i = 0; i < n_allocations; ++i) {
                transfer_requests[i] =
                        available_transfer_volume * requesting_utilities_rofs[i]
                        / sum_rofs * average_pipe_capacity
                        / available_transfer_volume;
            }

            /// Calculate allocations and flow rates through inter-utility
            /// connections.
            flow_rates_and_allocations = solve_QP(transfer_requests,
                                                  available_transfer_volume,
                                                  min_transfer_volume,
                                                  week);
            conveyed_volumes = vector<double>(
                    flow_rates_and_allocations.begin(),
                    flow_rates_and_allocations.begin() + n_pipes);

//            allocations.clear();
//            for (int id : utilities_ids)
//                allocations.push_back((double &&) flow_rates_and_allocations.at(
//                        (unsigned long) (n_pipes + id)));
            allocations = vector<double>(flow_rates_and_allocations.begin() +
                                         n_pipes,
                                         flow_rates_and_allocations.end());
            allocations[source_utility_id] = -allocations[source_utility_id];

            /// Mitigate demands.
            double sum_allocations = 0;
            int price_week = Utils::weekOfTheYear(week);
//            for (int i = 0; i < allocations.size(); ++i) {
//                if (i != source_utility_id) {
//                    int id = util_id_to_vertex_id->at(i);
//                    realization_utilities[id]->setDemand_offset
//                            (allocations[i], source_utility->waterPrice
//                                    (price_week));
//                    sum_allocations += allocations[i];
//                }
//
//                /// Remove transferred water from source of transfer
//                if (i != source_utility_id)
//                    transfer_water_source->removeWater(i, allocations[i]);
//            }

            for (auto u : *util_id_to_vertex_id) {
                int id = u.first;
                realization_utilities[u.second]->setDemand_offset
                        (allocations[id],
                         source_utility->waterPrice
                                 (price_week));
                sum_allocations += allocations[id];
                transfer_water_source->removeWater(id,
                                                   allocations[id]);
            }
        }
    }
}

/**
 * Solves Quadratic Programming problem to find optimal allocations given
 * transfer requests and constraints in conveyance capacities.
 * @param allocation_requests allocations requested by each utility (0 for
 * utilities not requesting transfers).
 * @param available_transfer_volume Total volume available for transfer from
 * source utility.
 * @param min_transfer_volume minimum transfer to be made to each utility.
 * @return
 */
vector<double> Transfers::solve_QP(
        vector<double> allocation_requests,
        double available_transfer_volume, double min_transfer_volume,
        int week) {

    vector<double> flow_rates_and_allocations;
    auto n_allocations = (unsigned int) allocation_requests.size();
    unsigned int n_vars = f.size();
    unsigned int n_pipes = n_vars - n_allocations - 1;
    Vector<double> x;

    Matrix<double> G = this->H;

    /// Set g0 vector to allocated to 2 * target_allocation.
    for (int i = 0; i < allocation_requests.size(); ++i) {
        f[n_pipes + buyers_ids[i]] = -2 * allocation_requests[i];
    }

    /// Set allocation bounds to 0 if no allocation is requested, and to
    /// available transfer volume otherwise.
    for (int i = 0; i < n_allocations; ++i) {
        if (allocation_requests[i] == NONE) {
            lb[n_pipes + buyers_ids[i]] = NONE;
            ub[n_pipes + buyers_ids[i]] = NONE;
        } else {
            double allocation_available = transfer_water_source
                    ->getAvailableAllocatedVolume(buyers_ids[i]);
            lb[n_pipes + buyers_ids[i]] = min(min_transfer_volume,
                                              allocation_available);
//            ub[n_pipes + buyers_ids[i]] = available_transfer_volume;
            ub[n_pipes + buyers_ids[i]] = min(
                    realization_utilities[i]->getUnrestrictedDemand(week) *
                    realization_utilities[i]->getDemand_multiplier(),
                    allocation_available);
        }
    }

    /// Set upper bound of the last utility (source) to total amount of water
    /// available. This allows for a smaller transferred volume, in case
    /// pipes cannot convey full amount available, but sets the cap as total
    /// amount available.
    ub[n_pipes + source_utility_id] = available_transfer_volume;

    /// Prevent minimum allocation from being more than capacity of pipes
    /// connected to utility. If it happens, decrease it to half of the sum of
    /// adjacent pipes capacities so that it avoid issues with bottle-necks
    /// through the network. The factor of 2 can be changed to any number
    /// smaller than 1.
    double max_flow;
    for (int i = 0; i < n_allocations + 1; ++i) { // skip source
        if (i != source_utility_id) {
            max_flow = 0;
            for (int j = 0; j < n_pipes; ++j) {
                max_flow += abs(Aeq[i][j] * ub[j]);
            }
            if (lb[n_pipes + i] > max_flow)
                lb[n_pipes + i] = max_flow / 2;
        }
    }

    /// Initialize allocations vector with 0s.
    x.resize(0, n_vars);

    /// Run quadratic programming solver.
    solve_quadprog_matlab_syntax(G, f, Aeq, beq, A, b, lb, ub, x);


//    print_matrix("H", G);
//    print_vector("f", f);
//    print_matrix("Aeq", Aeq);
//    print_vector("beq", beq);
//    print_vector("lb", lb);
//    print_vector("ub", ub);
//    print_vector("x", x);
//    cout << "======================================================" << endl;


    /// Convert Vector x into a vector.
    for (int i = 0; i < x.size(); ++i) {
        flow_rates_and_allocations.push_back(x[i]);
    }

    return flow_rates_and_allocations;
}

const vector<double> &Transfers::getAllocations() const {
    return allocations;
}

void Transfers::setRealization(unsigned int realization_id) {

}
