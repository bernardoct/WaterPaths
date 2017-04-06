//
// Created by bct52 on 2/1/17.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include "DataCollector.h"


DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                             const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                             int number_of_realizations) {
    for (int r = 0; r < number_of_realizations; ++r) {

        /// Add vector of structs corresponding the realization r.
        utilities_t.push_back(vector<Utility_t>());
        reservoir_t.push_back(vector<WaterSource_t>());
        restriction_policy_t.push_back(vector<RestrictionPolicy_t>());

        /// Add structs to realization.
        for (Utility *u : utilities) {
            utilities_t[r].push_back(*new Utility_t(u->id, u->getTotal_storage_capacity(), u->name));
        }
        for (WaterSource *ws : water_sources) {
            reservoir_t[r].push_back(*new WaterSource_t(ws->id, ws->getCapacity(), ws->name));
        }
        for (DroughtMitigationPolicy *dmp : drought_mitigation_policies) {
            restriction_policy_t[r].push_back(*new RestrictionPolicy_t(dmp->id));
        }
    }
    int i = 0;
}

/**
 * Collect data from realization step.
 * @param continuity_model_realization
 */
void DataCollector::collectData(ContinuityModelRealization *continuity_model_realization, int week) {

    Utility *u;
    WaterSource *ws;
    Restrictions *rp;
    int r = continuity_model_realization->realization_id;

    /// Get utilities data.
    for (int i = 0; i < continuity_model_realization->getUtilities().size(); ++i) {
        u = continuity_model_realization->getUtilities()[i];
        utilities_t[r][i].combined_storage.push_back(u->getStorageToCapacityRatio() *
                                                     u->getTotal_storage_capacity());
        utilities_t[r][i].rof.push_back(u->getRisk_of_failure());
        utilities_t[r][i].demand.push_back(u->getDemand(week));
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
        rp = (Restrictions *) continuity_model_realization->getDrought_mitigation_policies()[i];
        restriction_policy_t[r][i].restriction_multiplier.push_back(rp->getCurrent_multiplier());
    }
}

void DataCollector::printReservoirOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < reservoir_t.size(); ++r) {

        /// Print realization number.
        outStream << "Realization " << r << endl;
        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            outStream << reservoir_t[r][i].name << " ";
        }

        /// Print realization header.
        outStream << endl
                  << setw(8) << "Week";

        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            outStream
                    << setw(16) << "Av_vol."
                    << setw(10) << "Demands"
                    << setw(10) << "Up_spil"
                    << setw(10) << "Catc_Q"
                    << setw(10) << "Out_Q";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < reservoir_t[0][0].available_volume.size(); ++w) {
            outStream << setw(8) << w;
            for (int i = 0; i < reservoir_t[r].size(); ++i) {
                outStream
                        << setw(16) << setprecision(4) << reservoir_t[r][i].available_volume[w]
                        << setw(10) << setprecision(4) << reservoir_t[r][i].demands[w]
                        << setw(10) << setprecision(4) << reservoir_t[r][i].total_upstream_sources_inflows[w]
                        << setw(10) << setprecision(4) << reservoir_t[r][i].total_catchments_inflow[w]
                        << setw(10) << setprecision(4) << reservoir_t[r][i].outflows[w];
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

    for (int r = 0; r < utilities_t.size(); ++r) {

        /// Print realization number.
        outStream << "Realization " << r << endl;
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << utilities_t[r][i].name << " ";
        }

        /// Print realization header.
        outStream << endl << setw(8) << "Week";
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << setw(16) << "Sto_vol"
                      << setw(8) << "ROF"
                      << setw(8) << "Demand"
                      << setw(8) << "C. fund";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < utilities_t[0][0].rof.size(); ++w) {
            outStream << setw(8) << w;
            for (int i = 0; i < utilities_t[r].size(); ++i) {
                outStream << setw(16) << setprecision(4) << utilities_t[r][i].combined_storage[w]
                          << setw(8) << setprecision(4) << utilities_t[r][i].rof[w]
                          << setw(8) << setprecision(4) << utilities_t[r][i].demand[w]
                          << setw(8) << setprecision(4) << utilities_t[r][i].contingency_fund_size[w];
            }
            outStream << endl;
        }
    }

    outStream.close();
}

void DataCollector::printPoliciesOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < restriction_policy_t.size(); ++r) {

        if (restriction_policy_t[r].size() > 0) {
            /// Print realization number.
            outStream << "Realization " << r << endl;
            for (int i = 0; i < restriction_policy_t[r].size(); ++i) {
                outStream << restriction_policy_t[r][i].utility_id << " ";
            }

            /// Print realization header.
            outStream << endl << setw(8) << "Week";
            for (int i = 0; i < restriction_policy_t[r].size(); ++i) {
                outStream << setw(8) << "Multip.";
            }
            outStream << endl;

            /// Print numbers.
            for (unsigned long w = 0; w < restriction_policy_t[0][0].restriction_multiplier.size(); ++w) {
                outStream << setw(8) << w;
                for (int i = 0; i < restriction_policy_t[r].size(); ++i) {
                    outStream << setw(8) << setprecision(4) << restriction_policy_t[r][i].restriction_multiplier.at(w);
                }
                outStream << endl;
            }
        }
    }

    outStream.close();
}

