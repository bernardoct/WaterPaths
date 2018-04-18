//
// Created by David on 4/9/2018.
//

#include <iostream>
#include <numeric>
#include "JointWTP.h"

JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> participating_utilities, const double total_treatment_capacity,
                   const double total_capital_cost, vector<double> fixed_allocations,
                   double fixed_external_allocation, const vector<double> &construction_time_range,
                   double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED,
                      participating_utilities, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, NONE),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          total_added_capacity(total_treatment_capacity),
          total_capital_cost(total_capital_cost),
          external_allocation(fixed_external_allocation),  fixed_utility_allocations(&fixed_allocations),
          external_demands(nullptr), annual_demands(nullptr),
          third_party_ids(nullptr), third_party_sales_rates(nullptr), utility_peaking_factors(nullptr) {

    double temp_total_allocation = accumulate(fixed_utility_allocations->begin(),
                                              fixed_utility_allocations->end(),0.0) +
                                   external_allocation;
    if (temp_total_allocation > 1) {
        cout << "Normalizing fixed allocations" << endl;
        for (int u = 0; u < fixed_utility_allocations->size(); ++u)
            fixed_utility_allocations->at(u) = fixed_utility_allocations->at(u)/temp_total_allocation;
        external_allocation = external_allocation/temp_total_allocation;
    }
}


JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> participating_utilities, const double total_treatment_capacity,
                   const double total_capital_cost, const vector<double> &construction_time_range,
                   double permitting_period,
                   vector<double> demand_peaking_factor,
                   vector<vector<double>> future_annual_utility_demands,
                   vector<double> external_annual_demands)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED,
                      participating_utilities, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, NONE),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          total_added_capacity(total_treatment_capacity),
          total_capital_cost(total_capital_cost),
          external_allocation(0), fixed_utility_allocations(nullptr),
          external_demands(&external_annual_demands), annual_demands(&future_annual_utility_demands),
          third_party_ids(nullptr), third_party_sales_rates(nullptr), utility_peaking_factors(&demand_peaking_factor) {

    /// Normalize annual initial allocations based on relative demands
    /// incorporating peaking factors in each annual period
    /// TODO: ADD CONSIDERATION OF TAKE-OR-PAY CONTRACTS, REQUIRED MINIMUM PURCHASES
    double temp_total_annual_demand = 0.0;
    for (int y = 0; y < external_demands->size(); y++) {
        for (int u = 0; u < annual_demands->at(y).size(); u++) {
            temp_total_annual_demand += annual_demands->at(y).at(u)*utility_peaking_factors->at(u);
        }
        temp_total_annual_demand += external_demands->at(y)*
                utility_peaking_factors->at(utility_peaking_factors->size());

        for (int u = 0; u < annual_demands->at(y).size(); u++) {
            utility_allocation_fractions->at(y).at(u) = annual_demands->at(y).at(u)/temp_total_annual_demand;
        }
        utility_allocation_fractions->at(y).at(utility_allocation_fractions->at(y).size()+1) =
                external_demands->at(y)/temp_total_annual_demand;
    }
}

JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> participating_utilities, const double total_treatment_capacity,
                   const double total_capital_cost, vector<double> fixed_allocations,
                   double fixed_external_allocation, const vector<double> &construction_time_range,
                   double permitting_period, vector<int> third_party_utilities,
                   vector<double> third_party_sales_volumetric_rate)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED,
                      participating_utilities, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, NONE),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          total_added_capacity(total_treatment_capacity),
          total_capital_cost(total_capital_cost),
          external_allocation(fixed_external_allocation),  fixed_utility_allocations(&fixed_allocations),
          external_demands(nullptr), annual_demands(nullptr),
          third_party_ids(&third_party_utilities), third_party_sales_rates(&third_party_sales_volumetric_rate),
          utility_peaking_factors(nullptr) {

    double temp_total_allocation = accumulate(fixed_utility_allocations->begin(),
                                              fixed_utility_allocations->end(),0.0) +
                                   external_allocation;
    if (temp_total_allocation > 1) {
        cout << "Normalizing fixed allocations" << endl;
        for (int u = 0; u < fixed_utility_allocations->size(); ++u)
            fixed_utility_allocations->at(u) = fixed_utility_allocations->at(u)/temp_total_allocation;
        external_allocation = external_allocation/temp_total_allocation;
    }
}

JointWTP::JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
                   vector<int> participating_utilities, const double total_treatment_capacity,
                   const double total_capital_cost, const vector<double> &construction_time_range,
                   double permitting_period,
                   vector<double> demand_peaking_factor,
                   vector<vector<double>> future_annual_utility_demands,
                   vector<double> external_annual_demands, vector<int> third_party_utilities,
                   vector<double> third_party_sales_volumetric_rate)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED,
                      participating_utilities, NEW_JOINT_WTP,
                      construction_time_range, permitting_period, NONE),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          contract_type(contract_type),
          total_added_capacity(total_treatment_capacity),
          total_capital_cost(total_capital_cost),
          external_allocation(0),  fixed_utility_allocations(nullptr),
          external_demands(&external_annual_demands), annual_demands(&future_annual_utility_demands),
          third_party_ids(&third_party_utilities), third_party_sales_rates(&third_party_sales_volumetric_rate),
          utility_peaking_factors(&demand_peaking_factor) {

    /// Normalize annual initial allocations based on relative demands
    /// incorporating peaking factors in each annual period
    /// TODO: ADD CONSIDERATION OF TAKE-OR-PAY CONTRACTS, REQUIRED MINIMUM PURCHASES
    double temp_total_annual_demand = 0.0;
    for (int y = 0; y < external_demands->size(); y++) {
        for (int u = 0; u < annual_demands->at(y).size(); u++) {
            temp_total_annual_demand += annual_demands->at(y).at(u)*utility_peaking_factors->at(u);
        }
        temp_total_annual_demand += external_demands->at(y)*
                                    utility_peaking_factors->at(utility_peaking_factors->size());

        for (int u = 0; u < annual_demands->at(y).size(); u++) {
            utility_allocation_fractions->at(y).at(u) = annual_demands->at(y).at(u)/temp_total_annual_demand;
        }
        utility_allocation_fractions->at(y).at(utility_allocation_fractions->at(y).size()+1) =
                external_demands->at(y)/temp_total_annual_demand;
    }
}


/**
 * Copy constructor.
 * @param reservoir
 */
JointWTP::JointWTP(const JointWTP &joint_water_treatment_plant)
        : WaterSource(joint_water_treatment_plant),
          parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID),
          contract_type(joint_water_treatment_plant.contract_type),
          total_added_capacity(joint_water_treatment_plant.total_added_capacity),
          total_capital_cost(joint_water_treatment_plant.total_capital_cost),
          external_allocation(joint_water_treatment_plant.external_allocation),
          fixed_utility_allocations(joint_water_treatment_plant.fixed_utility_allocations),
          external_demands(joint_water_treatment_plant.external_demands),
          annual_demands(joint_water_treatment_plant.annual_demands),
          third_party_ids(joint_water_treatment_plant.third_party_ids),
          third_party_sales_rates(joint_water_treatment_plant.third_party_sales_rates),
          utility_peaking_factors(joint_water_treatment_plant.utility_peaking_factors),
          initial_capacity_added(joint_water_treatment_plant.initial_capacity_added),
          previous_year_allocated_capacities(joint_water_treatment_plant.previous_year_allocated_capacities) {

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

void JointWTP::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                               vector<double> &demand_outflow) {
    cout << "Can't go here, in JointWTP class" << endl;
    __throw_logic_error("Joint WTP class only meant to adjust allocations and demand targets, not continuity");
}

double JointWTP::calculateFixedAllocationCosts(int utility_id) {
    construction_cost_of_capital = total_capital_cost * (fixed_utility_allocations->at(utility_id));
    return construction_cost_of_capital;
}

int JointWTP::getContractType() {
    return contract_type;
}

vector<double> JointWTP::calculateAdjustableAllocationConstructionCosts(int utility_id, double bond_length) {
    vector<double> annual_capital_responsibility;
    for (int year = 0; year < bond_length; year++) {
        annual_capital_responsibility[year] =
                total_capital_cost/bond_length * utility_allocation_fractions->at(year).at(utility_id);
    }

    return annual_capital_responsibility;
}

double JointWTP::calculateNetPresentConstructionCost(
        int week, int utility_id, double discount_rate,
        vector<double>& debt_service_payments, double bond_term,
        double bond_interest_rate) const {
    double rate = bond_interest_rate / BOND_INTEREST_PAYMENTS_PER_YEAR;
    double net_present_cost_at_issuance = 0.0;

    /// Based on type of contract, debt service payments are determined here annually
    /// and the entire cost of the project for a given utility is present valued as well

    if (contract_type == 0) {
        /// Determine annual fixed payment
        double fixed_annual_responsibility = total_capital_cost * (fixed_utility_allocations->at(utility_id));

        /// Set total PV debt service total
        for (int year = 0; year < bond_term; year++)
            net_present_cost_at_issuance += fixed_annual_responsibility / pow((1 + discount_rate), year);

        debt_service_payments.emplace_back(bond_term, fixed_annual_responsibility);
            // vector of annual costs for the project for a utility

    } else if (contract_type == 1) {
        vector<double> annual_capital_responsibility;
        int present_value_year = 0;
        for (int year = week/WEEKS_IN_YEAR; year < bond_term + week/WEEKS_IN_YEAR; year++) {
            /// Determine annual payments
            annual_capital_responsibility[present_value_year] =
                    total_capital_cost/bond_term * utility_allocation_fractions->at(year).at(utility_id) *
                            (rate * pow(1. + rate, bond_term)) / (pow(1. + rate, bond_term) - 1.);

            /// Set total PV debt service total
            net_present_cost_at_issuance += annual_capital_responsibility[year] /
                    pow((1 + discount_rate), present_value_year);
                // TODO: this isn't right, figure out how fix present valuation

            debt_service_payments.push_back(annual_capital_responsibility[present_value_year]);
                // build vector of annual costs for the project for a utility
            present_value_year += 1;
        }
    } else if (contract_type == 2) {
        vector<double> annual_capital_responsibility;
        int present_value_year = 0;

        /// This is a problem, payments can't be known beforehand (not based on projections)
        // TODO: ACTUALLY DEAL WITH CHANGING DEBT SERVICE CALCULATIONS SO THAT THEY DONT HAPPEN RIGHT AT CONSTRUCTION
        for (int year = week/WEEKS_IN_YEAR; year < bond_term + week/WEEKS_IN_YEAR; year++) {
            /// Determine annual payments
            if (isOnline())
                annual_capital_responsibility[present_value_year] =
                        total_capital_cost/bond_term *
                                previous_year_allocated_capacities->at(utility_id)/total_added_capacity *
                        (rate * pow(1. + rate, bond_term)) / (pow(1. + rate, bond_term) - 1.);
            else
                annual_capital_responsibility[present_value_year] =
                        total_capital_cost/bond_term *
                                utility_allocation_fractions->at(year).at(utility_id) *
                        (rate * pow(1. + rate, bond_term)) / (pow(1. + rate, bond_term) - 1.);
                // for payments before plant comes online, they are based on use projections

            /// Set total PV debt service total
            net_present_cost_at_issuance += annual_capital_responsibility[year] /
                                            pow((1 + discount_rate), present_value_year);
            // TODO: this isn't right, figure out how fix present valuation

            debt_service_payments.push_back(annual_capital_responsibility[present_value_year]);
            // build vector of annual costs for the project for a utility
            present_value_year += 1;
        }
    } else if (contract_type == 3) {

    } else {

    }

    /// Check for errors.
    if (std::isnan(net_present_cost_at_issuance) || std::isnan(debt_service_payments[0])) {
        char error[512];
        sprintf(error, "rate: %f\n"
                        "principal: %f\n"
                        "# of payments: %f\n"
                        "level debt service payment: %f\n"
                        "net present cost at issuance: %f\n"
                        "week: %d\n"
                        "utility ID: %d\n",
                rate, total_capital_cost, bond_term, debt_service_payments[0],
                net_present_cost_at_issuance, week, utility_id);
        throw_with_nested(runtime_error(error));
    }

    /// Return NPC discounted from the time of issuance to the present.
    return net_present_cost_at_issuance;// / pow(1 + discount_rate, week / WEEKS_IN_YEAR);
}


/**
 * Returns the capacity to be installed for a given utility and deducts it
 * from maximum planned expansion.
 * @param utility_id
 * @return
 */
double JointWTP::implementFixedTreatmentCapacity(int utility_id) {
    return total_added_capacity * fixed_utility_allocations->at(utility_id);
}

/**
 * Returns the capacity allocation for a given utility for a given year
 * @param utility_id
 * @return
 */
double JointWTP::getAdjustableTreatmentCapacity(int utility_id, int year) {
    return total_added_capacity * utility_allocation_fractions->at(year).at(utility_id);
}

void JointWTP::updateTreatmentAllocations(int week) {
    /// MEANT TO ONLY RUN DURING ANNUAL LONG TERM ROF CALCULATION

    if (isOnline()) {
        cout << "Updating allocations for " << name << " in week "<< week << endl;
        cout << "Contract type: " << contract_type << endl;

        if (contract_type == 0) {
            /// fixed allocations, so treatment capacity is added to existing capacity per utility
            /// ALREADY DONE IN CONSTRUCTION HANDLER BECAUSE IT IS ONLY NECESSARY ONCE
            cout << "Treatment capacity and allocations are set in JointWTP construction handler." << endl;

        } else if (contract_type == 1) {
            /// uniform rate BASED ON USE PROJECTIONS
            /// first year of new plant, allocation assignment is done in construction handler
            /// every year following is done here:
            /// must adjust utility treatment capacities based on difference between those for the current
            /// year and the year before, then recalculate allocation fractions

            cout << "Current year estimate: " << week/WEEKS_IN_YEAR << endl;

            for (unsigned long i = 0; i < annual_demands->at(0).size(); ++i) {
                int u = utilities_with_allocations->at(i);

                allocated_treatment_capacities[u] += total_added_capacity *
                        (utility_allocation_fractions->at(week/WEEKS_IN_YEAR-1).at(u) -
                                utility_allocation_fractions->at(week/WEEKS_IN_YEAR).at(u));
                    // difference in this and previous year's allocation
                    // begins year after source goes online

                (*this->utilities_with_allocations)[i] = u;

                allocated_treatment_fractions[u] =  allocated_treatment_capacities[u]/total_treatment_capacity;
            }

        } else if (contract_type == 2) {
            /// SQUARE ONE REALLOCATION BASED ON PREVIOUS YEAR ACTUAL USE, NOT PROJECTION
            /// initial year allocations will be set based on utility projection levels in construction handler
            /// for all future years (here), allocations are based on average use of the attached source
            /// ATTRIBUTED TO THIS WTP
            /// THIS IS IMPORTANT take if a utility already has treatment capacity at a different wtp for the same
            /// source when this joint wtp is built, then its use of the source will always be enormous. need to judge
            /// use based on use above previously existing treatment capacity

            cout << "Current year estimate: " << week/WEEKS_IN_YEAR << endl;

            double total_past_source_demand = accumulate(annual_source_demand.begin(),annual_source_demand.end(),0.0);
            for (unsigned long i = 0; i < annual_demands->at(0).size(); ++i) {
                int u = utilities_with_allocations->at(i);

                double average_past_annual_demand = annual_source_demand[u]/WEEKS_IN_YEAR/7;
                    // right now, only set up to collect one year of past data
                    // results in avg MGD demand on this wtp

                /// Does this utility have an allocation on this source other than the joint wtp in question?
                double treatment_capacity_difference = (allocated_treatment_capacities[u] -
                        previous_year_allocated_capacities->at(u))/7;
                    // if this value is greater than 0, utility u also draws from the attached source from another wtp
                    // converted to MGD

                /// Account for other treatment capacities on the source, convert to MGW treatment capacity
                if (treatment_capacity_difference > 0) {
                    allocated_treatment_capacities[u] += (average_past_annual_demand - treatment_capacity_difference) *
                                                         utility_peaking_factors->at(u) * 7 -
                                                         previous_year_allocated_capacities->at(u);
                        // year to follow is allocated capacity based on past year's use multiplied by a peaking factor

                    previous_year_allocated_capacities->at(u) = (average_past_annual_demand - treatment_capacity_difference) *
                                                                utility_peaking_factors->at(u) * 7;
                } else {
                    allocated_treatment_capacities[u] += (average_past_annual_demand *
                                                          utility_peaking_factors->at(u) * 7) -
                                                         previous_year_allocated_capacities->at(u);
                    previous_year_allocated_capacities->at(u) = average_past_annual_demand *
                                                                utility_peaking_factors->at(u) * 7;
                }

                if (allocated_treatment_capacities[u] < 0)
                    allocated_treatment_capacities[u] = 0.0;

                (*this->utilities_with_allocations)[i] = u;

                allocated_treatment_fractions[u] = allocated_treatment_capacities[u] / total_treatment_capacity;
            }

            double temp = accumulate(previous_year_allocated_capacities->begin(),
                                     previous_year_allocated_capacities->end(),0.0);
            if (temp/total_added_capacity > 1) {
                cout << "Capacity was over-allocated based on past year's demands. Normalizing now..." << endl;
                cout << "Week: " << week << ", Allocated Capacity this time: " << temp << endl;
                cout << "Total Available Capacity: " << total_added_capacity << endl;

                for (int i = 0; i < previous_year_allocated_capacities->size(); i++) {
                    allocated_treatment_capacities[i] -= previous_year_allocated_capacities->at(i);

                    previous_year_allocated_capacities->at(i) =
                            previous_year_allocated_capacities->at(i) / temp * total_added_capacity;

                    allocated_treatment_capacities[i] += previous_year_allocated_capacities->at(i);
                    allocated_treatment_fractions[i] = allocated_treatment_capacities[i] / total_treatment_capacity;
                }
            }

        } else if (contract_type == 3) {

        } else {

        }
    }

    /// Reset annual counter
    for (unsigned long i = 0; i < annual_source_demand.size(); ++i)
        annual_source_demand[i] = 0.0;
}

double JointWTP::getAddedTotalTreatmentCapacity() {
    double total_capacity_to_add = 0.0;
    if (!initial_capacity_added)
        total_capacity_to_add = total_added_capacity;

    initial_capacity_added = true;

    return total_capacity_to_add;
}

void JointWTP::recordAllocationAdjustment(double added_treatment_capacity, int utility_id) {
    previous_year_allocated_capacities->at(utility_id) = added_treatment_capacity;
}

JointWTP::~JointWTP() = default;