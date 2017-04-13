//
// Created by bct52 on 2/1/17.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include "DataCollector.h"
#include "../DroughtMitigationInstruments/Transfers.h"

/*
 *
 */

DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                             const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                             int number_of_realizations) : number_of_realizations(number_of_realizations) {
    for (int r = 0; r < number_of_realizations; ++r) {

        /// Add vector of structs corresponding the realization r.
        utilities_t.push_back(vector<Utility_t>());
        reservoir_t.push_back(vector<WaterSource_t>());
        restriction_policy_t.push_back(vector<RestrictionPolicy_t>());
        transfers_policy_t.push_back(vector<Transfers_policy_t>());

        /// Add structs to realization.
        for (Utility *u : utilities) {
            utilities_t[r].push_back(*new Utility_t(u->id, u->getTotal_storage_capacity(), u->name));
        }
        for (WaterSource *ws : water_sources) {
            reservoir_t[r].push_back(*new WaterSource_t(ws->id, ws->getCapacity(), ws->name));
        }
        for (DroughtMitigationPolicy *dmp : drought_mitigation_policies) {
            if (dmp->type == RESTRICTIONS)
                restriction_policy_t[r].push_back(*new RestrictionPolicy_t(dmp->id));
            else if (dmp->type == TRANSFERS)
                transfers_policy_t[r].push_back(*new Transfers_policy_t(
                        dmp->id, dynamic_cast<Transfers *>(dmp)->getUtilities_ids()));
        }
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
        utilities_t[r][i].combined_storage.push_back(u->getStorageToCapacityRatio() *
                                                     u->getTotal_storage_capacity());
        utilities_t[r][i].rof.push_back(u->getRisk_of_failure());
        utilities_t[r][i].unrestricted_demand.push_back(u->getUnrestrictedDemand(week));
        utilities_t[r][i].restricted_demand.push_back(u->getRestrictedDemand(week));
        utilities_t[r][i].contingency_fund_size.push_back(u->getContingency_fund());
    }

    /// Get reservoirs data.
    for (int i = 0; i < continuity_model_realization->getWater_sources().size(); ++i) {
        ws = continuity_model_realization->getWater_sources()[i];
        reservoir_t[r][i].available_volume.push_back(ws->getAvailable_volume());
        reservoir_t[r][i].demands.push_back(ws->getDemand());
        reservoir_t[r][i].total_upstream_sources_inflows.push_back(ws->getUpstream_source_inflow());
        reservoir_t[r][i].outflows.push_back(ws->getTotal_outflow());
        reservoir_t[r][i].total_catchments_inflow.push_back(ws->getCatchment_upstream_catchment_inflow());
    }

    /// Get drought mitigation policy data.
    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == RESTRICTIONS) {
            rp = (Restrictions *) continuity_model_realization->getDrought_mitigation_policies()[i];
            restriction_policy_t[r][rp->id].restriction_multiplier.push_back(rp->getCurrent_multiplier());
        }
    }

    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == TRANSFERS) {
            tp = (Transfers *) continuity_model_realization->getDrought_mitigation_policies()[i];
            transfers_policy_t[r][tp->id].demand_offsets.push_back(tp->getAllocations());
        }
    }
}

void DataCollector::printReservoirOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < number_of_realizations; ++r) {

        /// Print realization number.
        outStream << "Realization " << r << endl;
        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            outStream << reservoir_t[r][i].name << " ";
        }

        /// Print realization header.
        outStream << endl
                  << setw(COLUMN_WIDTH) << "Week";

        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            outStream
                    << setw(2 * COLUMN_WIDTH) << "Av_vol."
                    << setw(COLUMN_WIDTH) << "Demands"
                    << setw(COLUMN_WIDTH) << "Up_spil"
                    << setw(COLUMN_WIDTH) << "Catc_Q"
                    << setw(COLUMN_WIDTH) << "Out_Q";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < reservoir_t[0][0].available_volume.size(); ++w) {
            outStream << setw(COLUMN_WIDTH) << w;
            for (int i = 0; i < reservoir_t[r].size(); ++i) {
                outStream
                        << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[r][i].available_volume[w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[r][i].demands[w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[r][i].total_upstream_sources_inflows[w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[r][i].total_catchments_inflow[w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << reservoir_t[r][i].outflows[w];
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

    for (int r = 0; r <number_of_realizations; ++r) {

        /// Print realization number.
        outStream << "Realization " << r << endl;
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << utilities_t[r][i].name << " ";
        }

        /// Print realization header.
        outStream << endl << setw(COLUMN_WIDTH) << "Week";
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << setw(2 * COLUMN_WIDTH) << "Stored"
                      << setw(COLUMN_WIDTH) << " "
                      << setw(COLUMN_WIDTH) << "Rest."
                      << setw(COLUMN_WIDTH) << "Unrest."
                      << setw(COLUMN_WIDTH) << "Conting.";
        }
        outStream << endl << setw(COLUMN_WIDTH) << " ";
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << setw(2 * COLUMN_WIDTH) << "Volume"
                      << setw(COLUMN_WIDTH) << "ROF"
                      << setw(COLUMN_WIDTH) << "Demand"
                      << setw(COLUMN_WIDTH) << "Demand"
                      << setw(COLUMN_WIDTH) << "Fund";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < utilities_t[0][0].rof.size(); ++w) {
            outStream << setw(COLUMN_WIDTH) << w;
            for (int i = 0; i < utilities_t[r].size(); ++i) {
                outStream << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << utilities_t[r][i].combined_storage[w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << utilities_t[r][i].rof[w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << utilities_t[r][i].restricted_demand[w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << utilities_t[r][i].unrestricted_demand[w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << utilities_t[r][i].contingency_fund_size[w];
            }
            outStream << endl;
        }
    }

    outStream.close();
}

void DataCollector::printPoliciesOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < number_of_realizations; ++r) {
        if (restriction_policy_t[r].size() > 0) {
            /// Print realization number.
            outStream << endl;
            outStream << "Realization " << r << endl;
            outStream << setw(COLUMN_WIDTH) << " ";
            for (int i = 0; i < restriction_policy_t[r].size(); ++i) {
                outStream << setw(COLUMN_WIDTH - 1) << "Restr. " << restriction_policy_t[r][i].utility_id;
            }
            for (int i = 0; i < transfers_policy_t[r].size(); ++i) {
                for (int buyer_id : transfers_policy_t[r][i].utilities_ids)
                    outStream << setw(COLUMN_WIDTH - 1) << "Transf. " << transfers_policy_t[r][i].transfer_policy_id;
            }

            /// Print realization header.
            outStream << endl << setw(COLUMN_WIDTH) << "Week";
            for (int i = 0; i < restriction_policy_t[r].size(); ++i) {
                outStream << setw(COLUMN_WIDTH) << "Multip.";
            }
            for (int i = 0; i < transfers_policy_t[r].size(); ++i) {
                for (int buyer_id : transfers_policy_t[r][i].utilities_ids)
                    outStream << setw(COLUMN_WIDTH - 1) << "Alloc. " << buyer_id;
            }
            outStream << endl;

            /// Print numbers.
            for (unsigned long w = 0; w < restriction_policy_t[0][0].restriction_multiplier.size(); ++w) {
                outStream << setw(COLUMN_WIDTH) << w;
                for (int i = 0; i < restriction_policy_t[r].size(); ++i) {
                    outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << restriction_policy_t[r][i].restriction_multiplier.at(w);
                }
                for (int i = 0; i < transfers_policy_t[r].size(); ++i) {
                    for (double &a : transfers_policy_t[r][i].demand_offsets.at(w))
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << a;
                }
                outStream << endl;
            }
        }
    }

    outStream.close();
}

