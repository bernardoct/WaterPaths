//
// Created by bct52 on 2/1/17.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "DataCollector.h"
#include "../DroughtMitigationInstruments/Transfers.h"

/*
 *
 */

DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                             const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                             int number_of_realizations) : number_of_realizations(number_of_realizations) {

    for (Utility *u : utilities) {
        utilities_t.push_back(Utility_t(u->id, u->getTotal_storage_capacity(), u->name));
    }

    for (WaterSource *ws : water_sources) {
        reservoir_t.push_back(WaterSource_t(ws->id, ws->getCapacity(), ws->name));
    }

    for (DroughtMitigationPolicy *dmp : drought_mitigation_policies) {
        if (dmp->type == RESTRICTIONS)
            restriction_policy_t.push_back(RestrictionPolicy_t(dmp->id));
        else if (dmp->type == TRANSFERS)
            transfers_policy_t.push_back(Transfers_policy_t(
                    dmp->id, dynamic_cast<Transfers *>(dmp)->getUtilities_ids()));
    }

    for (int r = 0; r < number_of_realizations; ++r) {
        for (Utility_t &ut : utilities_t) {
            ut.restricted_demand.push_back(vector<double>());
            ut.combined_storage.push_back(vector<double>());
            ut.unrestricted_demand.push_back(vector<double>());
            ut.contingency_fund_size.push_back(vector<double>());
            ut.rof.push_back(vector<double>());
        }

        for (WaterSource_t &wst : reservoir_t) {
            wst.available_volume.push_back(vector<double>());
            wst.demands.push_back(vector<double>());
            wst.outflows.push_back(vector<double>());
            wst.total_catchments_inflow.push_back(vector<double>());
            wst.total_upstream_sources_inflows.push_back(vector<double>());
        }

        for (RestrictionPolicy_t &rp : restriction_policy_t) {
            rp.restriction_multiplier.push_back(vector<double>());
        }

        for (Transfers_policy_t &tp : transfers_policy_t)
            tp.demand_offsets.push_back(vector<vector<double>>());
    }
}

/**
 * Collect data from realization step.
 * @param continuity_model_realization
 */
void DataCollector::collectData(ContinuityModelRealization *continuity_model_realization, int week) {

    Utility *u;
    WaterSource *ws;
    Restrictions *rp;
    Transfers *tp;
    DroughtMitigationPolicy *dmp;
    int r = continuity_model_realization->realization_id;

    /// Get utilities data.
    for (int i = 0; i < continuity_model_realization->getUtilities().size(); ++i) {
        u = continuity_model_realization->getUtilities()[i];
        utilities_t[i].combined_storage[r].push_back(u->getStorageToCapacityRatio() *
                                                     u->getTotal_storage_capacity());
        utilities_t[i].rof[r].push_back(u->getRisk_of_failure());
        utilities_t[i].unrestricted_demand[r].push_back(u->getUnrestrictedDemand());
        utilities_t[i].restricted_demand[r].push_back(u->getRestrictedDemand());
        utilities_t[i].contingency_fund_size[r].push_back(u->getContingency_fund());
        utilities_t[i].net_present_infrastructure_cost = u->getInfrastructure_net_present_cost();
    }

    /// Get reservoirs data.
    for (int i = 0; i < continuity_model_realization->getWater_sources().size(); ++i) {
        ws = continuity_model_realization->getWater_sources()[i];
        reservoir_t[i].available_volume[r].push_back(ws->getAvailable_volume());
        reservoir_t[i].demands[r].push_back(ws->getDemand());
        reservoir_t[i].total_upstream_sources_inflows[r].push_back(ws->getUpstream_source_inflow());
        reservoir_t[i].outflows[r].push_back(ws->getTotal_outflow());
        reservoir_t[i].total_catchments_inflow[r].push_back(ws->getCatchment_upstream_catchment_inflow());
    }

    /// Get drought mitigation policy data.
    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == RESTRICTIONS) {
            rp = (Restrictions *) continuity_model_realization->getDrought_mitigation_policies()[i];
            restriction_policy_t[rp->id].restriction_multiplier[r].push_back(rp->getCurrent_multiplier());
        }
    }

    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == TRANSFERS) {
            tp = (Transfers *) continuity_model_realization->getDrought_mitigation_policies()[i];
            transfers_policy_t[tp->id].demand_offsets[r].push_back(tp->getAllocations());
        }
    }
}

void DataCollector::printReservoirOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < number_of_realizations; ++r) {

        /// Print realization number.
        outStream << "Realization " << r << endl;
        for (int i = 0; i < reservoir_t.size(); ++i) {
            outStream << reservoir_t[r].name << " ";
        }

        /// Print realization header.
        outStream << endl
                  << setw(COLUMN_WIDTH) << "Week";

        for (int i = 0; i < reservoir_t.size(); ++i) {
            outStream
                    << setw(2 * COLUMN_WIDTH) << "Av_vol."
                    << setw(COLUMN_WIDTH) << "Demands"
                    << setw(COLUMN_WIDTH) << "Up_spil"
                    << setw(COLUMN_WIDTH) << "Catc_Q"
                    << setw(COLUMN_WIDTH) << "Out_Q";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < reservoir_t[0].available_volume.size(); ++w) {
            outStream << setw(COLUMN_WIDTH) << w;
            for (int u = 0; u < reservoir_t.size(); ++u) {
                outStream
                        << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << reservoir_t[u].available_volume[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[u].demands[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << reservoir_t[u].total_upstream_sources_inflows[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << reservoir_t[u].total_catchments_inflow[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[u].outflows[r][w];
            }
            outStream << endl;
        }
        outStream << endl << endl;
    }

    outStream.close();
}

void DataCollector::printUtilityOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < number_of_realizations; ++r) {

        /// Print realization number.
        outStream << endl;
        outStream << "Realization " << r << endl;
        for (int i = 0; i < utilities_t.size(); ++i) {
            outStream << utilities_t[i].name << " ";
        }

        /// Print realization header.
        outStream << endl << setw(COLUMN_WIDTH) << " ";
        for (int i = 0; i < utilities_t.size(); ++i) {
            outStream << setw(2 * COLUMN_WIDTH) << "Stored"
                      << setw(COLUMN_WIDTH) << " "
                      << setw(COLUMN_WIDTH) << "Rest."
                      << setw(COLUMN_WIDTH) << "Unrest."
                      << setw(COLUMN_WIDTH) << "Conting.";
        }
        outStream << endl << setw(COLUMN_WIDTH) << "Week";
        for (int i = 0; i < utilities_t.size(); ++i) {
            outStream << setw(2 * COLUMN_WIDTH) << "Volume"
                      << setw(COLUMN_WIDTH) << "ROF"
                      << setw(COLUMN_WIDTH) << "Demand"
                      << setw(COLUMN_WIDTH) << "Demand"
                      << setw(COLUMN_WIDTH) << "Fund";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < utilities_t[0].rof[0].size(); ++w) {
            outStream << setw(COLUMN_WIDTH) << w;
            for (int u = 0; u < utilities_t.size(); ++u) {
                outStream << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].combined_storage[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << utilities_t[u].rof[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].restricted_demand[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].unrestricted_demand[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].contingency_fund_size[r][w];
            }
            outStream << endl;
        }

        for (int u = 0; u < utilities_t.size(); ++u) {
            outStream << utilities_t[u].name << " NPC of infrastructure: " << utilities_t[u]
                    .net_present_infrastructure_cost << endl;
        }
        outStream << endl;
    }

    /// print objectives
    for (Utility_t ut : utilities_t) {
        outStream << ut.name << " ";
        for (double o : ut.objectives) {
            outStream << "\t" << "Reliability: " << o << endl;
        }
    }

    outStream.close();
}

void DataCollector::printPoliciesOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < number_of_realizations; ++r) {
        if (restriction_policy_t.size() > 0) {
            /// Print realization number.
            outStream << endl;
            outStream << "Realization " << r << endl;
            outStream << setw(COLUMN_WIDTH) << " ";
            for (int i = 0; i < restriction_policy_t.size(); ++i) {
                outStream << setw(COLUMN_WIDTH - 1) << "Restr. " << restriction_policy_t[i].utility_id;
            }
            for (int i = 0; i < transfers_policy_t.size(); ++i) {
                for (int buyer_id : transfers_policy_t[i].utilities_ids)
                    outStream << setw(COLUMN_WIDTH - 1) << "Transf. " << transfers_policy_t[i].transfer_policy_id;
            }

            /// Print realization header.
            outStream << endl << setw(COLUMN_WIDTH) << "Week";
            for (int i = 0; i < restriction_policy_t.size(); ++i) {
                outStream << setw(COLUMN_WIDTH) << "Multip.";
            }
            for (int i = 0; i < transfers_policy_t.size(); ++i) {
                for (int buyer_id : transfers_policy_t[i].utilities_ids)
                    outStream << setw(COLUMN_WIDTH - 1) << "Alloc. " << buyer_id;
            }
            outStream << endl;

            /// Print numbers.
            for (unsigned long w = 0; w < restriction_policy_t[0].restriction_multiplier.size(); ++w) {
                outStream << setw(COLUMN_WIDTH) << w;
                for (int i = 0; i < restriction_policy_t.size(); ++i) {
                    outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                              << restriction_policy_t[i].restriction_multiplier[r].at(w);
                }
                for (int i = 0; i < transfers_policy_t.size(); ++i) {
                    for (double &a : transfers_policy_t[i].demand_offsets[r].at(w))
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << a;
                }
                outStream << endl;
            }
        }
    }

    outStream.close();
}


/**
 * Calculates reliability objective
 * @param utility
 * @return reliability.
 */
double DataCollector::calculateReliabilityObjective(Utility_t utility) {
    unsigned long n_realizations = utility.combined_storage.size();
    unsigned long n_weeks = utility.combined_storage[0].size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    vector<vector<int>> realizations_year_reliabilities(n_realizations, vector<int>(n_years, 0));
    vector<int> year_reliabilities(n_years, 0);

    /// Creates a table with years that failed in each realization.
    for (int r = 0; r < n_realizations; ++r) {
        for (int y = 0; y < n_years; ++y) {
            for (int w = (int) round(y * WEEKS_IN_YEAR); w < (int) round((y + 1) * WEEKS_IN_YEAR); ++w) {
                if (utility.combined_storage[r][w] / utility.capacity < STORAGE_CAPACITY_RATIO_FAIL) {
                    realizations_year_reliabilities[r][y] = FAILURE;
                }
            }
        }
    }

    /// Creates a vector with the number of realizations that failed for each year.
    for (int y = 0; y < n_years; ++y) {
        for (int r = 0; r < n_realizations; ++r) {
            if (realizations_year_reliabilities[r][y] == FAILURE) year_reliabilities[y]++;
        }
    }

    double check_non_zero = accumulate(year_reliabilities.rbegin(), year_reliabilities.rend(), 0);

    /// Returns year with most realization failures, divided by the number of realizations (reliability objective).
    if (check_non_zero > 0)
        return 1. - (double) *max_element(year_reliabilities.begin(), year_reliabilities.end()) / n_realizations;
    else
        return 1.;
}

void DataCollector::calculateObjectives() {
    for (Utility_t &ut : utilities_t) {
        ut.objectives.push_back(calculateReliabilityObjective(ut));
    }
}