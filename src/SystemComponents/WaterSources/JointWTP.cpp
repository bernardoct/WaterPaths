//
// Created by David on 4/9/2018.
//

#include <iostream>
#include <numeric>
#include "JointWTP.h"
#include "../../Utils/Utils.h"


JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> *utils_with_allocations,
                   vector<int> connected_sources, const double added_treatment_capacity,
                   vector<Bond *> &bonds, vector<double> &fixed_allocations,
                   double fixed_external_allocation, const vector<double> &construction_time_range,
                   double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, added_treatment_capacity,
                      connected_sources, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, bonds),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          external_allocation(fixed_external_allocation) {

    utilities_with_allocations = utils_with_allocations;

    /// Set identifiers to make code more readable
    /// reminder: there can only be one external utility
    EXTERNAL_SOURCE_ID = (int) utilities_with_allocations->size();
    N_UTILITIES_INCLUDING_EXTERNAL = (int) utilities_with_allocations->size() + 1;
    N_INTERNAL_UTILITIES = N_UTILITIES_INCLUDING_EXTERNAL - 1;

    for (unsigned long u : *utilities_with_allocations) {
        allocated_treatment_fractions.push_back(fixed_allocations[u]);
        allocated_treatment_capacities.push_back(fixed_allocations[u]*total_treatment_capacity);
        previous_period_allocated_capacities.push_back(allocated_treatment_capacities[u]);
    }
    previous_period_allocated_capacities.push_back(external_allocation*total_treatment_capacity);

    current_external_allocation_fraction = external_allocation;

    double temp_total_allocation = accumulate(fixed_allocations.begin(),
                                              fixed_allocations.end(),0.0) +
                                   external_allocation;
    if (temp_total_allocation > 1) {
        cout << "Normalizing fixed allocations" << endl;
        for (unsigned long u : *utilities_with_allocations) {
            allocated_treatment_fractions[u] = fixed_allocations.at(u) / temp_total_allocation;
            allocated_treatment_capacities[u] = allocated_treatment_fractions[u]*total_treatment_capacity;
        }
        external_allocation = external_allocation/temp_total_allocation;
        previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] =
                external_allocation*total_treatment_capacity;
    }
}


JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> *utils_with_allocations,
                   vector<int> connected_sources, const double added_treatment_capacity,
                   vector<Bond *> &bonds, const vector<double> &construction_time_range,
                   double permitting_period,
                   vector<double> demand_peaking_factor,
                   vector<vector<double>> future_annual_utility_demands,
                   vector<double> external_annual_demands, const int past_demand_weeks_to_use)
        : WaterSource(name, id, vector<Catchment *>(), NONE, added_treatment_capacity,
                      connected_sources, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, bonds),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          external_allocation(NON_INITIALIZED),
          external_demands(external_annual_demands), annual_demands(future_annual_utility_demands),
          utility_peaking_factors(demand_peaking_factor),
          observed_demand_weeks_to_use(past_demand_weeks_to_use) {

    /// Normalize annual initial allocations based on relative demands
    /// incorporating peaking factors in each annual period

    utilities_with_allocations = utils_with_allocations;

    /// Set identifiers to make code more readable
    /// reminder: there can only be one external utility
    EXTERNAL_SOURCE_ID = (int) utilities_with_allocations->size();
    N_UTILITIES_INCLUDING_EXTERNAL = (int) utilities_with_allocations->size() + 1;
    N_INTERNAL_UTILITIES = N_UTILITIES_INCLUDING_EXTERNAL - 1;
    PROJECTED_YEARS = (int) external_demands.size();

    utility_allocation_fractions = std::vector<vector<double>>(N_UTILITIES_INCLUDING_EXTERNAL,
                                                               vector<double>(PROJECTED_YEARS));

    adjusted_allocated_treatment_capacities = vector<double>(N_UTILITIES_INCLUDING_EXTERNAL,0.0);

    setProjectedAllocationFractions(utility_allocation_fractions,
                                    annual_demands,
                                    external_demands,
                                    utility_peaking_factors);

    /// Set first year values
    for (unsigned long u : *utilities_with_allocations) {
        allocated_treatment_fractions.push_back(utility_allocation_fractions.at(u)[0]);
        allocated_treatment_capacities.push_back(allocated_treatment_fractions[u]*total_treatment_capacity);
        previous_period_allocated_capacities.push_back(allocated_treatment_capacities[u]);

        parent_reservoir_treatment_capacities.push_back(0);
    }
    previous_period_allocated_capacities.push_back(utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[0]
                                                   * total_treatment_capacity);
    current_external_allocation_fraction = utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[0];
}

JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> *utils_with_allocations,
                   vector<int> connected_sources, const double added_treatment_capacity,
                   vector<Bond *> &bonds, const vector<double> &construction_time_range,
                   double permitting_period,
                   vector<double> demand_peaking_factor,
                   vector<vector<double>> future_annual_utility_demands,
                   vector<double> external_annual_demands)
        : WaterSource(name, id, vector<Catchment *>(), NONE, added_treatment_capacity,
                      connected_sources, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, bonds),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          external_allocation(NON_INITIALIZED),
          external_demands(external_annual_demands), annual_demands(future_annual_utility_demands),
          utility_peaking_factors(demand_peaking_factor) {

    /// Normalize annual initial allocations based on relative demands
    /// incorporating peaking factors in each annual period

    utilities_with_allocations = utils_with_allocations;

    /// Set identifiers to make code more readable
    /// reminder: there can only be one external utility
    EXTERNAL_SOURCE_ID = (int) utilities_with_allocations->size();
    N_UTILITIES_INCLUDING_EXTERNAL = (int) utilities_with_allocations->size() + 1;
    N_INTERNAL_UTILITIES = N_UTILITIES_INCLUDING_EXTERNAL - 1;
    PROJECTED_YEARS = (int) external_demands.size();

    utility_allocation_fractions = std::vector<vector<double>>(N_UTILITIES_INCLUDING_EXTERNAL,
                                                               vector<double>(PROJECTED_YEARS));

    setProjectedAllocationFractions(utility_allocation_fractions,
                                    annual_demands,
                                    external_demands,
                                    utility_peaking_factors);

    /// Set first year values
    for (unsigned long u : *utilities_with_allocations) {
        allocated_treatment_fractions.push_back(utility_allocation_fractions.at(u)[0]);
        allocated_treatment_capacities.push_back(allocated_treatment_fractions[u]*total_treatment_capacity);
        previous_period_allocated_capacities.push_back(allocated_treatment_capacities[u]);
    }
    previous_period_allocated_capacities.push_back(utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[0]
                                                   * total_treatment_capacity);
    current_external_allocation_fraction = utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[0];
}

JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> *utils_with_allocations,
                   vector<int> connected_sources, const double added_treatment_capacity,
                   vector<Bond *> &bonds, vector<double> &fixed_allocations,
                   double fixed_external_allocation, const vector<double> &construction_time_range,
                   double permitting_period, vector<int> third_party_utilities,
                   vector<double> third_party_sales_volumetric_rate)
        : WaterSource(name, id, vector<Catchment *>(), NONE, added_treatment_capacity,
                      connected_sources, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, bonds),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          external_allocation(fixed_external_allocation),
          third_party_ids(third_party_utilities), third_party_sales_rates(third_party_sales_volumetric_rate) {

    /// TODO: ADD CONSIDERATION OF TAKE-OR-PAY CONTRACTS, REQUIRED MINIMUM PURCHASES
    utilities_with_allocations = utils_with_allocations;

    /// Set identifiers to make code more readable
    /// reminder: there can only be one external utility
    EXTERNAL_SOURCE_ID = (int) utilities_with_allocations->size();
    N_UTILITIES_INCLUDING_EXTERNAL = (int) utilities_with_allocations->size() + 1;
    N_INTERNAL_UTILITIES = N_UTILITIES_INCLUDING_EXTERNAL - 1;

    for (unsigned long u : *utilities_with_allocations) {
        allocated_treatment_fractions.push_back(fixed_allocations[u]);
        allocated_treatment_capacities.push_back(fixed_allocations[u]*total_treatment_capacity);
        previous_period_allocated_capacities.push_back(allocated_treatment_capacities[u]);
    }
    previous_period_allocated_capacities.push_back(external_allocation*total_treatment_capacity);

    current_external_allocation_fraction = external_allocation;

    double temp_total_allocation = accumulate(fixed_allocations.begin(),
                                              fixed_allocations.end(),0.0) +
                                   external_allocation;
    if (temp_total_allocation > 1) {
        cout << "Normalizing fixed allocations" << endl;
        for (unsigned long u : *utilities_with_allocations) {
            allocated_treatment_fractions[u] = fixed_allocations.at(u) / temp_total_allocation;
            allocated_treatment_capacities[u] = allocated_treatment_fractions[u]*total_treatment_capacity;
        }
        external_allocation = external_allocation/temp_total_allocation;
        previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] =
                external_allocation*total_treatment_capacity;
    }
}

JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> *utils_with_allocations,
                   vector<int> connected_sources, const double added_treatment_capacity,
                   vector<Bond *> &bonds, const vector<double> &construction_time_range,
                   double permitting_period,
                   vector<double> demand_peaking_factor,
                   vector<vector<double>> future_annual_utility_demands,
                   vector<double> external_annual_demands, vector<int> third_party_utilities,
                   vector<double> third_party_sales_volumetric_rate)
        : WaterSource(name, id, vector<Catchment *>(), NONE, added_treatment_capacity,
                      connected_sources, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, bonds),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          external_allocation(NON_INITIALIZED),
          external_demands(external_annual_demands), annual_demands(future_annual_utility_demands),
          third_party_ids(third_party_utilities), third_party_sales_rates(third_party_sales_volumetric_rate),
          utility_peaking_factors(demand_peaking_factor) {

    /// Normalize annual initial allocations based on relative demands
    /// incorporating peaking factors in each annual period

    utilities_with_allocations = utils_with_allocations;

    /// Set identifiers to make code more readable
    /// reminder: there can only be one external utility
    EXTERNAL_SOURCE_ID = (int) utilities_with_allocations->size();
    N_UTILITIES_INCLUDING_EXTERNAL = (int) utilities_with_allocations->size() + 1;
    N_INTERNAL_UTILITIES = N_UTILITIES_INCLUDING_EXTERNAL - 1;
    PROJECTED_YEARS = (int) external_demands.size();

    utility_allocation_fractions = std::vector<vector<double>>(N_UTILITIES_INCLUDING_EXTERNAL,
                                                               vector<double>(PROJECTED_YEARS));

    setProjectedAllocationFractions(utility_allocation_fractions,
                                    annual_demands,
                                    external_demands,
                                    utility_peaking_factors);

    /// Set first year values
    for (unsigned long u : *utilities_with_allocations) {
        allocated_treatment_fractions.push_back(utility_allocation_fractions.at(u)[0]);
        allocated_treatment_capacities.push_back(allocated_treatment_fractions[u]*total_treatment_capacity);
        previous_period_allocated_capacities.push_back(allocated_treatment_capacities[u]);
    }
    previous_period_allocated_capacities.push_back(utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[0]
                                                   * total_treatment_capacity);
    current_external_allocation_fraction = utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[0];
}


/**
 * Copy constructor.
 * @param reservoir
 */
JointWTP::JointWTP(const JointWTP &joint_water_treatment_plant)
        : WaterSource(joint_water_treatment_plant),
          parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID),
          contract_type(joint_water_treatment_plant.contract_type),
          external_allocation(joint_water_treatment_plant.external_allocation),
          external_demands(joint_water_treatment_plant.external_demands),
          annual_demands(joint_water_treatment_plant.annual_demands),
          third_party_ids(joint_water_treatment_plant.third_party_ids),
          third_party_sales_rates(joint_water_treatment_plant.third_party_sales_rates),
          utility_peaking_factors(joint_water_treatment_plant.utility_peaking_factors),
          initial_capacity_added(joint_water_treatment_plant.initial_capacity_added),
          previous_period_allocated_capacities(joint_water_treatment_plant.previous_period_allocated_capacities),
          build_year(joint_water_treatment_plant.build_year),
          utility_allocation_fractions(joint_water_treatment_plant.utility_allocation_fractions),
          observed_demand_weeks_to_use(joint_water_treatment_plant.observed_demand_weeks_to_use),
          EXTERNAL_SOURCE_ID(joint_water_treatment_plant.EXTERNAL_SOURCE_ID),
          N_UTILITIES_INCLUDING_EXTERNAL(joint_water_treatment_plant.N_UTILITIES_INCLUDING_EXTERNAL),
          N_INTERNAL_UTILITIES(joint_water_treatment_plant.N_INTERNAL_UTILITIES),
          PROJECTED_YEARS(joint_water_treatment_plant.PROJECTED_YEARS),
          current_external_allocation_fraction(joint_water_treatment_plant.current_external_allocation_fraction) {

}


/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
JointWTP &JointWTP::operator=(const JointWTP &joint_water_treatment_plant) {
    WaterSource::operator=(joint_water_treatment_plant);
    return *this;
}

JointWTP::~JointWTP() = default;

void JointWTP::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                               vector<double> &demand_outflow) {
    cout << "Can't go here, in JointWTP class" << endl;
    __throw_logic_error("Joint WTP class only meant to adjust allocations and demand targets, not continuity");
}

int JointWTP::getContractType() {
    return contract_type;
}


/**
 * Returns the capacity to be installed for a given utility and deducts it
 * from maximum planned expansion.
 * @param utility_id
 * @return
 */
double JointWTP::implementFixedTreatmentCapacity(int utility_id) {
    return total_treatment_capacity * allocated_treatment_fractions[utility_id];
}

/**
 * Returns the capacity allocation for a given utility for a given year
 * @param utility_id
 * @return
 */
double JointWTP::getAdjustableTreatmentCapacity(int utility_id, int year) {
    double trtmt_cap = 0.0;

    /// If contract type is 2, meaning allocations are set based on demands on plant,
    /// set the first year allocations based on long-term projections so that
    /// demand can accrue to the plant (if set to 0, it will stay at 0 forever)
    if (contract_type == 1) {
        trtmt_cap =
                total_treatment_capacity
                * utility_allocation_fractions.at((unsigned long)utility_id)[year];
    } else if (contract_type == 2)
        trtmt_cap =
                total_treatment_capacity
                * utility_allocation_fractions.at((unsigned long)utility_id)[PROJECTED_YEARS-1];

    return trtmt_cap;
}


/**
 * Returns the capacity allocation for a given utility
 * @param utility_id
 * @return
 */
double JointWTP::getAllocatedTreatmentCapacity(int utility_id) const {

    if (utility_id == WATER_QUALITY_ALLOCATION)
        __throw_invalid_argument("Water quality pool does not have allocated treatment "
                                         "capacity.");
    return allocated_treatment_capacities[utility_id];
}


/**
 * Returns the capacity allocation for a given utility
 * @param utility_id
 * @return
 */
double JointWTP::getAllocatedTreatmentFraction(int utility_id) const {
    if (utility_id == WATER_QUALITY_ALLOCATION)
        __throw_invalid_argument("Water quality pool does not have allocated treatment "
                                         "fraction.");
    return allocated_treatment_fractions[utility_id];
}


/**
 * Returns the previous period's capacity allocations for this joint wtp.
 * If called in model after updateTreatmentAllocations is run during a timestep,
 * this vector should contain up-to-date allocation information
 * @return previous_period_allocated_capacities
 */
vector<double> JointWTP::getPreviousPeriodAllocatedCapacities() const {
    return previous_period_allocated_capacities;
}

void JointWTP::updateTreatmentAllocations(int week, vector<vector<vector<double>>> weekly_demands) {
    /// MEANT TO ONLY RUN DURING ANNUAL LONG TERM ROF CALCULATION

    double total_past_period_source_demand = 0.0;
    int past_weeks_to_use = observed_demand_weeks_to_use;

    if (isOnline()) {
        cout << "Updating allocations for " << name << " in week "<< week << endl;
        cout << "This is week " << WEEK_OF_YEAR[week] << " in year "<< Utils::yearOfTheRealization(week) << endl;
        cout << "Contract type: " << contract_type << endl;
        cout << "As a reminder, this Joint WTP was built in year " << build_year << endl;

        if (contract_type == 0) {
            /// fixed allocations, so treatment capacity is added to existing capacity per utility
            /// ALREADY DONE IN CONSTRUCTION HANDLER BECAUSE IT IS ONLY NECESSARY ONCE
            cout << "Treatment capacity and allocations are set in JointWTP construction handler." << endl;
            cout << "For year " << Utils::yearOfTheRealization(week) << ", those allocations are: " << endl;
            for (int u : *utilities_with_allocations) {
                cout << "Utility " << u << ", Allocation fraction: " << allocated_treatment_fractions[u] << endl;
                previous_period_allocated_capacities.at((unsigned long)u) = total_treatment_capacity *
                        allocated_treatment_fractions[u];
            }
            cout << "External allocation fraction: " << current_external_allocation_fraction << endl;

        } else if (contract_type == 1) {
            /// uniform rate BASED ON USE PROJECTIONS
            /// first year of new plant, allocation assignment is done in construction handler
            /// every year following is done here:
            /// must adjust utility treatment capacities based on allocations for the current
            /// year, then recalculate allocation fractions

            cout << "Current year: " << Utils::yearOfTheRealization(week) << endl;
            for (int u : *utilities_with_allocations) {
                cout << "Utility " << u << ", Allocation fraction (before adjustment): "
                     << allocated_treatment_fractions[u] << endl;
                cout << "Utility " << u << ", Allocation capacity (before adjustment): "
                     << allocated_treatment_capacities[u] << endl;
            }
            cout << "External Allocation (year before): "
                 << utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[Utils::yearOfTheRealization(week)-1] << endl;

            for (unsigned long i = 0; i < utilities_with_allocations->size(); ++i) {
                int u = utilities_with_allocations->at(i);

                allocated_treatment_capacities[u] = total_treatment_capacity *
                                utility_allocation_fractions.at((unsigned long)u)[Utils::yearOfTheRealization(week)];

                (*this->utilities_with_allocations)[i] = u;

                allocated_treatment_fractions[u] =  allocated_treatment_capacities[u]/total_treatment_capacity;

                previous_period_allocated_capacities.at((unsigned long)u) = total_treatment_capacity *
                        utility_allocation_fractions.at((unsigned long)u)[Utils::yearOfTheRealization(week)];
            }

            current_external_allocation_fraction =
                    utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[Utils::yearOfTheRealization(week)];

            previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] =
                    current_external_allocation_fraction * total_treatment_capacity;

            for (int u : *utilities_with_allocations) {
                cout << "Utility " << u << ", Allocation fraction (after adjustment): "
                     << allocated_treatment_fractions[u] << endl;
                cout << "Utility " << u << ", Allocation capacity (after adjustment): "
                     << allocated_treatment_capacities[u] << endl;
            }
            cout << "External Allocation (after): "
                 << utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[Utils::yearOfTheRealization(week)] << endl;

        } else if (contract_type == 2) {
            /// SQUARE ONE REALLOCATION BASED ON PREVIOUS YEAR ACTUAL USE, NOT PROJECTION
            /// initial year allocations will be set based on utility projection levels in construction handler
            /// for all future years (here), allocations are based on average use of the attached source
            /// ATTRIBUTED TO THIS WTP
            /// THIS IS IMPORTANT take if a utility already has treatment capacity at a different wtp for the same
            /// source when this joint wtp is built, then its use of the source will always be enormous. need to judge
            /// use based on use above previously existing treatment capacity

            cout << "Current year: " << Utils::yearOfTheRealization(week) << endl;

            for (int u : *utilities_with_allocations) {
                cout << "Utility " << u << ", Allocation fraction (before adjustment): "
                     << allocated_treatment_fractions[u] << endl;
            }
            cout << "External allocation fraction (before): " << current_external_allocation_fraction << endl;

            if (week-observed_demand_weeks_to_use < 0)
                past_weeks_to_use = week;

            /// Sum past demand on the source (this means the underlying reservoir) and apply peaking factor
            /// Here, factor in any demand from another WTP on the source by subtracting from summed demand
            /// available capacity that isn't from this WTP
            for (int& u : *utilities_with_allocations) {

                /// Calculate treatment capacity on parent source that this WTP can account for
                /// for a given utility u
                double parent_treatment_capacity_externally_covered;
                if (parent_reservoir_treatment_capacities[u] == 0) {
                    parent_treatment_capacity_externally_covered = 0;
                    total_past_period_source_demand += 0;
                    continue;
                } else {
                    parent_treatment_capacity_externally_covered =
                            parent_reservoir_treatment_capacities[u] - allocated_treatment_capacities[u];
                }

                if (parent_treatment_capacity_externally_covered < 0) {
                    parent_treatment_capacity_externally_covered = 0;
//                        cout << "Error in JointWTP updateTreatmentAllocations: "
//                             << "parent reservoir allocated treatment capacity "
//                                     "cannot be lower than allocated JointWTP capacity" << endl;
                }

                for (auto w = (int)(weekly_demands[parent_reservoir_ID][u].size() - past_weeks_to_use);
                     w < weekly_demands[parent_reservoir_ID][u].size(); w++) {

                    double wtp_demand_obligation = weekly_demands[parent_reservoir_ID][u][w]
                                                   - parent_treatment_capacity_externally_covered;

//                    if (wtp_demand_obligation > allocated_treatment_capacities[u])
//                        wtp_demand_obligation = allocated_treatment_capacities[u];
                    if (wtp_demand_obligation < 0)
                        wtp_demand_obligation = 0;

                    /// Total past period demand on source WTP, in average MGW
                    total_past_period_source_demand += wtp_demand_obligation
                                                       * utility_peaking_factors.at((unsigned long)u)
                                                       / past_weeks_to_use;
                }
            }

            /// Include demand projections from external utilities
            /// Assumed to be annual average MGD, so is converted to average MGW for most recent year
            total_past_period_source_demand +=
                    (external_demands[Utils::yearOfTheRealization(week)] * DAYS_IN_WEEK
                     * utility_peaking_factors.at(EXTERNAL_SOURCE_ID));

            /// Determine individual utility fractional use of joint wtp
            double average_past_demand;
            for (unsigned long i = 0; i < utilities_with_allocations->size(); i++) {
                average_past_demand = 0.0;
                int u = utilities_with_allocations->at(i);

                double parent_treatment_capacity_obligation;
                if (parent_reservoir_treatment_capacities[u] == 0) {
                    parent_treatment_capacity_obligation = 0;
                    average_past_demand += 0;
                    continue;
                } else {
                    parent_treatment_capacity_obligation = allocated_treatment_capacities[u]
                                                           / parent_reservoir_treatment_capacities[u];
                }

                for (auto w = (int)(weekly_demands[parent_reservoir_ID][u].size() - past_weeks_to_use);
                     w < weekly_demands[parent_reservoir_ID][u].size(); w++) {

                    /// Calculate ratio of treatment capacity on parent source that this WTP accounts for
                    /// for a given utility u
                    double wtp_demand_obligation = parent_treatment_capacity_obligation
                                                   * weekly_demands[parent_reservoir_ID][u][w];
//                    if (wtp_demand_obligation > allocated_treatment_capacities[u]) {
//                        wtp_demand_obligation = allocated_treatment_capacities[u];
//                    }

                    /// Collect single utility's avg MGW of demand on this WTP
                    average_past_demand += wtp_demand_obligation
                                           * utility_peaking_factors.at((unsigned long)u)
                                           / past_weeks_to_use;
                }

                /// Determine fraction of past demand attributed to utility u
                double past_demand_fraction;
                past_demand_fraction = average_past_demand / total_past_period_source_demand;

                /// Record current allocation fraction for each utility in the project
                utility_allocation_fractions.at((unsigned long)u)[Utils::yearOfTheRealization(week)] =
                        past_demand_fraction;

                allocated_treatment_capacities[u] = past_demand_fraction * total_treatment_capacity;

                previous_period_allocated_capacities.at((unsigned long)u) =
                        past_demand_fraction * total_treatment_capacity;

                if (previous_period_allocated_capacities.at((unsigned long)u) < 0)
                    cout << "Error in week " << week << ", allocated capacities for Joint WTP are negative." << endl;

                if (allocated_treatment_capacities[u] < 0)
                    allocated_treatment_capacities[u] = 0.0;

                if (allocated_treatment_fractions[u] < 0)
                    cout << "Error in JointWTP: allocated_treatment_fraction for utility " << u << " is < 0" << endl;

                (*this->utilities_with_allocations)[i] = u;

                allocated_treatment_fractions[u] = allocated_treatment_capacities[u] / total_treatment_capacity;

            }

            /// Calculate fraction and capacity for external allocation, assuming that there is no
            /// constraint on treatment capacity from parent source (right now, any external allocation
            /// in a joint wtp is part of the parent reservoir water quality pool)
            current_external_allocation_fraction = (external_demands[Utils::yearOfTheRealization(week)]
                                                    * DAYS_IN_WEEK
                                                    * utility_peaking_factors.at(EXTERNAL_SOURCE_ID))
                                                   / total_past_period_source_demand;
            previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] =
                    current_external_allocation_fraction * total_treatment_capacity;

            double temp = accumulate(previous_period_allocated_capacities.begin(),
                                     previous_period_allocated_capacities.end(),0.0);
                // this vector includes external allocation

            if (temp/total_treatment_capacity > 1) {
                cout << "Capacity was over-allocated in JointWTP based on past year's demands. Normalizing..." << endl;
                cout << "Week: " << week << ", Allocated Capacity this time: " << temp << endl;
                cout << "Total Available Capacity: " << total_treatment_capacity << endl;

                for (unsigned long i = 0; i < utilities_with_allocations->size(); i++) {
                    allocated_treatment_capacities[i] -= previous_period_allocated_capacities.at(i);

                    previous_period_allocated_capacities.at(i) =
                            previous_period_allocated_capacities.at(i) / temp * total_treatment_capacity;

                    allocated_treatment_capacities[i] += previous_period_allocated_capacities.at(i);
                    allocated_treatment_fractions[i] = allocated_treatment_capacities[i] / total_treatment_capacity;
                }
//                current_external_allocation_fraction = 1 - accumulate(allocated_treatment_fractions.begin(),
//                                                                      allocated_treatment_fractions.end(),
//                                                                      0.0);
                previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] =
                        previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] / temp * total_treatment_capacity;

                current_external_allocation_fraction =
                        previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] / total_treatment_capacity;
            }

            for (int u : *utilities_with_allocations) {
                cout << "Utility " << u << ", Allocation fraction (after adjustment): "
                     << allocated_treatment_fractions[u] << endl;
            }
            cout << "External allocation fraction (after adjustment): " << current_external_allocation_fraction << endl;

            cout << "Allocated capacities now sum to: "
                 << accumulate(previous_period_allocated_capacities.begin(),
                               previous_period_allocated_capacities.end(), 0.0) << endl;

            for (int u : *utilities_with_allocations) {
                cout << "Utility " << u << ", Allocation capacity (after adjustment): "
                     << allocated_treatment_capacities[u] << endl;
            }
            cout << "External allocation capacity (after adjustment): "
                 << previous_period_allocated_capacities[EXTERNAL_SOURCE_ID]  << endl;

        } else {

        }
    }
}

double JointWTP::getAddedTotalTreatmentCapacity() {
    double total_capacity_to_add = 0.0;
    if (!initial_capacity_added)
        total_capacity_to_add = total_treatment_capacity;

    initial_capacity_added = true;

    return total_capacity_to_add;
}

void JointWTP::recordAllocationAdjustment(double added_treatment_capacity, int utility_id) {
    previous_period_allocated_capacities.at((unsigned long)utility_id) = added_treatment_capacity;
}

void JointWTP::setBuildYear(int year) {
    build_year = year;
}


double JointWTP::calculateJointWTPDebtServiceFraction(int utility_id, int week) {

//    if (WEEK_OF_YEAR[week] == 0) {
//        cout << "allocated_treatment_fraction for utility " << utility_id << ": "
//             << allocated_treatment_fractions[utility_id] << endl;
//        cout << "utility_allocation_fraction for utility " << utility_id << ": "
//             << utility_allocation_fractions.at(utility_id)[Utils::yearOfTheRealization(week)] << endl;
//    }

    double total_allocated = 0.0;
    double utility_principal_responsibility_fraction;
    if (contract_type == 0) {
        /// fixed payments, based on initial allocations

        total_allocated += accumulate(allocated_treatment_fractions.begin(),
                                      allocated_treatment_fractions.end(), 0.0)
                           + external_allocation;

        utility_principal_responsibility_fraction = allocated_treatment_fractions[utility_id]/total_allocated;

    } else if (contract_type == 1) {
        /// adjustable payments, based on period use
        /// uniform rate applies here in that everyone is charged equal rate per unit use
        /// and allocation fractions were determined in the constructors based on
        /// relative expected use

        if (utility_allocation_fractions.at((unsigned long)utility_id)[Utils::yearOfTheRealization(week)]
            != allocated_treatment_fractions[utility_id] && isOnline() && WEEK_OF_YEAR[week] == 0
            && Utils::yearOfTheRealization(week) > build_year) {
            cout << "Error in JointWTP: allocated treatment fractions in year " << Utils::yearOfTheRealization(week)
                 << " for determining financial payment are not correct" << endl;
            cout << "Reminder: allocations are not updated by JointWTP class until one year after "
                 << "plant comes online" << endl;
        }

        /// If payments start before plant comes online, it is necessary to update allocations
        /// for debt service calculations, though it has no impact on water treatment or use
        for (int u = 0; u < utilities_with_allocations->size(); u++)
            allocated_treatment_fractions[u] =
                    utility_allocation_fractions.at((unsigned long)u)[Utils::yearOfTheRealization(week)];

        for (int u = 0; u < utilities_with_allocations->size(); u++)
            total_allocated += allocated_treatment_fractions[u];
        total_allocated +=
                utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[Utils::yearOfTheRealization(week)];

        /// If allocations fractions sum to <= 0, add factor to prevent dividing by zero below
        if (total_allocated <= 0) {
            cout << "Total allocated payments to active utilities is less than or equal to 0" << endl;
            total_allocated = 0.000001;
        }

        utility_principal_responsibility_fraction = allocated_treatment_fractions[utility_id]/total_allocated;

        if (WEEK_OF_YEAR[week] == 0 && isOnline()) {
            cout << "Financial responsibility in week " << week << " for utility " << utility_id
                 << " is " << utility_principal_responsibility_fraction << endl;

            cout << "Financial responsibility in week " << week << " for external utility "
                 << " is "
                 << utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[Utils::yearOfTheRealization(week)]/total_allocated
                 << endl;
        }

    } else if (contract_type == 2) {
        /// adjustable payments, based on period allocations that were based on historic use

        for (int& u : *utilities_with_allocations)
            total_allocated += allocated_treatment_capacities[u];
        total_allocated += external_demands[Utils::yearOfTheRealization(week)] * DAYS_IN_WEEK;

        /// If allocations fractions sum to <= 0, add factor to prevent dividing by zero below
        if (total_allocated <= 0) {
            total_allocated = 0.000001;
        }

        utility_principal_responsibility_fraction = allocated_treatment_capacities[utility_id]/total_allocated;

    } else {
        utility_principal_responsibility_fraction = 0;
    }

    if (utility_principal_responsibility_fraction < 0 && isOnline())
        cout << "During JointWTP debt service calculation, utility treatment fraction is < 0" << endl;
    else if (utility_principal_responsibility_fraction > 1 && isOnline())
        cout << "During JointWTP debt service calculation, utility treatment fraction is > 1" << endl;

    return utility_principal_responsibility_fraction;
}

int JointWTP::getParentSourceID() {
    return parent_reservoir_ID;
}

double JointWTP::getAllocationAdjustment(const int utility_id) {
    /// For contracts that do not do retroactive payments, the vector holding adjustments
    /// is not initialized in the constructor, meaning it is empty and so this information
    /// can be used to ensure no allocation adjustments happen when not expected.
    if (adjusted_allocated_treatment_capacities.empty())
        return 0;
    else
        return adjusted_allocated_treatment_capacities[utility_id]/total_treatment_capacity;
}

void JointWTP::setAllocationAdjustment(const int utility_id, const double allocation_adjustment) {
    if (!adjusted_allocated_treatment_capacities.empty())
        adjusted_allocated_treatment_capacities[utility_id] = allocation_adjustment;
}


void JointWTP::setProjectedAllocationFractions(vector<vector<double>> &utility_allocation_fractions,
                                               vector<vector<double>> &annual_demands,
                                               vector<double> &external_demands,
                                               vector<double> &utility_peaking_factors) {
    double expected_total_annual_demand = 0.0;
    for (unsigned long y = 0; y < PROJECTED_YEARS; y++) {
        expected_total_annual_demand = 0.0;
        for (int u : *utilities_with_allocations) {
            expected_total_annual_demand +=
                    annual_demands.at((unsigned long)u)[y]*utility_peaking_factors.at((unsigned long)u)*DAYS_IN_WEEK;
        }
        expected_total_annual_demand += external_demands.at(y) * DAYS_IN_WEEK *
                                        utility_peaking_factors.at(EXTERNAL_SOURCE_ID);

        /// If allocations sum to <= 0, add factor to prevent dividing by zero below
        /// because of new if statement below, it shouldn't matter anyway, but this is a cover just in case
        if (expected_total_annual_demand <= 0) {
            cout << "Projections in year " << y << " are less than or equal to zero." << endl;
            expected_total_annual_demand = 0.000001;
        }

        /// If annual total demands exceed treatment capacity of new project,
        /// normalize them relative to the expected demand. If not, normalize relative
        /// to the plant treatment capacity
        if (expected_total_annual_demand > total_treatment_capacity) {
            for (int u : *utilities_with_allocations) {
                utility_allocation_fractions.at((unsigned long)u)[y] =
                        (annual_demands.at((unsigned long)u)[y]
                         * utility_peaking_factors.at((unsigned long)u)*DAYS_IN_WEEK)
                        / expected_total_annual_demand;
            }
            utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[y] =
                    (external_demands.at(y)*utility_peaking_factors.at(EXTERNAL_SOURCE_ID)*DAYS_IN_WEEK)
                    / expected_total_annual_demand;
        } else {
            for (int u : *utilities_with_allocations) {
                utility_allocation_fractions.at((unsigned long)u)[y] =
                        (annual_demands.at((unsigned long)u)[y]
                         *utility_peaking_factors.at((unsigned long)u)*DAYS_IN_WEEK)
                        / total_treatment_capacity;
            }
            utility_allocation_fractions.at(EXTERNAL_SOURCE_ID)[y] =
                    (external_demands.at(y)*utility_peaking_factors.at(EXTERNAL_SOURCE_ID)*DAYS_IN_WEEK)
                    / total_treatment_capacity;
        }
    }

    /// Check values
    double temp_year_total_allocs;
    for (unsigned long y = 0; y < PROJECTED_YEARS; y++) {
        temp_year_total_allocs = 0.0;
        for (unsigned long u = 0; u < N_UTILITIES_INCLUDING_EXTERNAL; u++) {
            temp_year_total_allocs += utility_allocation_fractions.at(u)[y];
        }
        if (temp_year_total_allocs > 1)
            cout << "Error in JointWTP: utility allocation fractions in future year " << y << " sum to > 1." << endl;
    }

}


void JointWTP::setAllocatedTreatmentCapacity(const int year, const int utility_id) {
    /// See getAdjustableTreatmentCapacity for reason that allocations are set as such for
    /// contract type 2
    if (contract_type == 1) {
        allocated_treatment_fractions[utility_id] = utility_allocation_fractions[utility_id][year];
        allocated_treatment_capacities[utility_id] = allocated_treatment_fractions[utility_id]
                                                     * total_treatment_capacity;
    } else if (contract_type == 2) {
        allocated_treatment_fractions[utility_id] = utility_allocation_fractions[utility_id][PROJECTED_YEARS-1];
        allocated_treatment_capacities[utility_id] = allocated_treatment_fractions[utility_id]
                                                     * total_treatment_capacity;
    }
}

void JointWTP::recordParentReservoirTreatmentCapacity(const int utility_id, const double capacity_value) {
    if (!parent_reservoir_treatment_capacities.empty())
        parent_reservoir_treatment_capacities[utility_id] = capacity_value;
}

double JointWTP::getExternalDemand(const int year) {
    return external_demands[year] * DAYS_IN_WEEK;
}

double JointWTP::getExternalTreatmentAllocation() {
    return current_external_allocation_fraction;
}

void JointWTP::setExternalAllocatedTreatmentCapacity(const int year) {
    if (contract_type == 0) {
        previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] = external_allocation * total_treatment_capacity;
        current_external_allocation_fraction = external_allocation;
    } else {
        previous_period_allocated_capacities[EXTERNAL_SOURCE_ID] =
                utility_allocation_fractions[EXTERNAL_SOURCE_ID][year] * total_treatment_capacity;
        current_external_allocation_fraction =
                utility_allocation_fractions[EXTERNAL_SOURCE_ID][year];
    }
}