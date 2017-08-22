//
// Created by bct52 on 2/1/17.
//

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sys/stat.h>
#include <numeric>
#include "DataCollector.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "Utils.h"
#include "../DroughtMitigationInstruments/RawWaterReleases.h"


DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                             const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                             int number_of_realizations, Graph water_sources_graph) :
        number_of_realizations(number_of_realizations), water_sources_graph(water_sources_graph) {

    struct stat sb;
    if (stat(output_directory.c_str(),
             &sb) == 0 && S_ISDIR(sb.st_mode))
        cout << "Output will be printed to folder " << output_directory << endl;
    else {
//        cout << Utils::getexepath() << output_directory << endl;
        __throw_invalid_argument("Output folder does not exist.");
    }

    /// Create data utilities.
    for (Utility *u : utilities) {
        utilities_t.emplace_back(u->id,
                                 u->name);
    }

    /// Create data water sources.
    for (WaterSource *ws : water_sources) {
        water_sources_t.emplace_back(ws->id,
                                     ws->getCapacity(),
                                     ws->name);
    }

    /// Create data drought mitigation policies
    for (DroughtMitigationPolicy *dmp : drought_mitigation_policies) {
        if (dmp->type == RESTRICTIONS)
            restriction_policies_t.emplace_back(dmp->id);
        else if (dmp->type == TRANSFERS)
            transfers_policies_t.emplace_back(
                    dmp->id,
                    dynamic_cast<Transfers *>(dmp)->getUtilities_ids());
        else if (dmp->type == RAW_WATER_TRANSFERS)
            raw_water_transfer_policies_t.emplace_back(
                    dmp->id,
                    dynamic_cast<RawWaterReleases *>(dmp)->getUtilities_ids());
    }

    /// Create vectors to store weekly information.
    for (int r = 0; r < number_of_realizations; ++r) {
        for (Utility_t &ut : utilities_t) {
            ut.restricted_demand.emplace_back();
            ut.combined_storage.emplace_back();
            ut.unrestricted_demand.emplace_back();
            ut.contingency_fund_size.emplace_back();
            ut.net_present_infrastructure_cost.push_back((double &&) NONE);
            ut.gross_revenues.emplace_back();
            ut.debt_service_payments.emplace_back();
            ut.contingency_fund_contribution.emplace_back();
            ut.insurance_contract_cost.emplace_back();
            ut.insurance_payout.emplace_back();
            ut.st_rof.push_back(vector<double>());
            ut.lt_rof.push_back(vector<double>());
            ut.drought_mitigation_cost.emplace_back();
            ut.capacity.emplace_back();
        }

        for (WaterSource_t &wst : water_sources_t) {
            wst.available_volume.emplace_back();
            wst.demands.emplace_back();
            wst.outflows.emplace_back();
            wst.total_catchments_inflow.emplace_back();
            wst.total_upstream_sources_inflows.emplace_back();
            wst.evaporated_volume.emplace_back();
        }

        for (RestrictionPolicy_t &rp : restriction_policies_t) {
            rp.restriction_multiplier.emplace_back();
        }

        for (TransfersPolicy_t &tp : transfers_policies_t)
            tp.demand_offsets.emplace_back();

        for (RawWaterTransferPolicy_t &rwtp : raw_water_transfer_policies_t) {
            rwtp.raw_water_transfer_total.emplace_back();
            rwtp.utility_target_storage_ratio.emplace_back();
        }

        pathways.emplace_back();
    }
}

/**
 * Collect data from realization step.
 * @param continuity_model_realization
 */
void DataCollector::collectData(ContinuityModelRealization *continuity_model_realization) {

    Utility *u;
    WaterSource *ws;
    Restrictions *rp;
    Transfers *tp;
    RawWaterReleases *rwtp;
    DroughtMitigationPolicy *dmp;
    int r = continuity_model_realization->realization_id;
    vector<int> infra_build;

    /// Get utilities data.
    for (int i = 0; i < continuity_model_realization->getUtilities().size(); ++i) {
        u = continuity_model_realization->getUtilities()[i];
        utilities_t[i].combined_storage[r].push_back(u->getStorageToCapacityRatio() * u->getTotal_storage_capacity());
        utilities_t[i].lt_rof[r].push_back(u->getLong_term_risk_of_failure());
        utilities_t[i].st_rof[r].push_back(u->getRisk_of_failure());
        utilities_t[i].unrestricted_demand[r].push_back(u->getUnrestrictedDemand());
        utilities_t[i].restricted_demand[r].push_back(u->getRestrictedDemand());
        utilities_t[i].contingency_fund_size[r].push_back(u->getContingency_fund());
        utilities_t[i].net_present_infrastructure_cost[r] = u->getInfrastructure_net_present_cost();
        utilities_t[i].gross_revenues[r].push_back(u->getGrossRevenue());
        utilities_t[i].debt_service_payments[r].push_back(u->getCurrent_debt_payment());
        utilities_t[i].contingency_fund_contribution[r].push_back(u->getCurrent_contingency_fund_contribution());
        utilities_t[i].drought_mitigation_cost[r].push_back(u->getDrought_mitigation_cost());
        utilities_t[i].insurance_contract_cost[r].push_back(u->getInsurance_purchase());
        utilities_t[i].insurance_payout[r].push_back(u->getInsurance_payout());
        utilities_t[i].capacity[r].push_back(u->getTotal_storage_capacity());
        infra_build = u->getInfrastructure_built();
        if (!infra_build.empty()) pathways[r].push_back(infra_build);
    }

    /// Get reservoirs data.
    for (int i = 0; i < continuity_model_realization->getWater_sources().size(); ++i) {
        ws = continuity_model_realization->getWater_sources()[i];
        water_sources_t[i].available_volume[r].push_back(ws->getAvailable_volume());
        water_sources_t[i].demands[r].push_back(ws->getDemand());
        water_sources_t[i].total_upstream_sources_inflows[r].push_back(ws->getUpstream_source_inflow());
        water_sources_t[i].outflows[r].push_back(ws->getTotal_outflow());
        water_sources_t[i].total_catchments_inflow[r].push_back(ws->getUpstreamCatchmentInflow());
        water_sources_t[i].evaporated_volume[r].push_back(ws->getEvaporated_volume());
    }

    /// Get drought mitigation policy data.
    int p = 0;
    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == RESTRICTIONS) {
            rp = (Restrictions *) continuity_model_realization->getDrought_mitigation_policies()[i];
            restriction_policies_t[p].restriction_multiplier[r].push_back(rp->getCurrent_multiplier());
            p++;
        }
    }

    p = 0;
    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == TRANSFERS) {
            tp = (Transfers *) continuity_model_realization->getDrought_mitigation_policies()[i];
            transfers_policies_t[p].demand_offsets[r].push_back(tp->getAllocations());
            p++;
        }
    }

    p = 0;
    for (int i = 0; i < continuity_model_realization->getDrought_mitigation_policies().size(); ++i) {
        dmp = continuity_model_realization->getDrought_mitigation_policies()[i];
        if (dmp->type == RAW_WATER_TRANSFERS) {
            rwtp = (RawWaterReleases *) continuity_model_realization->getDrought_mitigation_policies()[i];
            raw_water_transfer_policies_t[p].raw_water_transfer_total[r].push_back(rwtp->getRawWaterTransferVolume());
            raw_water_transfer_policies_t[p].utility_target_storage_ratio[r].push_back(rwtp->getUtilityTargetStorageLevels());
            p++;
        }
    }

    for (Utility *uu : continuity_model_realization->getUtilities()) {
        uu->resetDataColletionVariables();
    }
}

void DataCollector::printPathways(string file_name) {
    std::ofstream outStream;
    outStream.open(output_directory + file_name);

    outStream << "Realization\tutility\tweek\tinfra." << endl;

    for (int r = 0; r < number_of_realizations; ++r)
        for (vector<int> &infra : pathways[r]) {
            outStream << r << "\t" << infra[0] << "\t" << infra[1] << "\t"
                      << infra[2] << endl;
        }

    outStream.close();
}


void DataCollector::printReservoirOutput(string file_name) {

    std::ofstream outStream;
    outStream.open(output_directory + file_name);
    int n_weeks = (int) water_sources_t[0].total_upstream_sources_inflows[0].size();

    for (int r = 0; r < number_of_realizations; ++r) {

        /// Print realization number.
        outStream << "Realization " << r << endl;
        outStream << endl
                  << setw(COLUMN_WIDTH) << " ";
        for (auto &ws : water_sources_t) {
            outStream << setw(7 * COLUMN_WIDTH) << ws.name;
        }

        /// Print realization header.
        outStream << endl
                  << setw(COLUMN_WIDTH) << "Week";

        for (int i = 0; i < water_sources_t.size(); ++i) {
            outStream
                    << setw(2 * COLUMN_WIDTH) << "Av_vol."
                    << setw(COLUMN_WIDTH) << "Demands"
                    << setw(COLUMN_WIDTH) << "Up_spil"
                    << setw(COLUMN_WIDTH) << "Catc_Q"
                    << setw(COLUMN_WIDTH) << "Evap_Q"
                    << setw(COLUMN_WIDTH) << "Out_Q";
        }
        outStream << endl;

        /// Print numbers.
        for (int w = 0; w < n_weeks; ++w) {
            outStream << setw(COLUMN_WIDTH) << w;
            for (auto &u : water_sources_t) {
                outStream
                        << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << u.available_volume[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << u.demands[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << u.total_upstream_sources_inflows[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << u.total_catchments_inflow[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << u.evaporated_volume[r][w]
                        << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                        << u.outflows[r][w];
            }
            outStream << endl;
        }
        outStream << endl << endl;
    }

    outStream.close();
}

void DataCollector::printReservoirOutputCompact(string file_name) {

    int n_weeks =
            (int) water_sources_t[0].total_upstream_sources_inflows[0].size();

#pragma omp parallel for
    for (int r = 0; r < number_of_realizations; ++r) {
        std::ofstream outStream;
        outStream.open(output_directory + file_name + std::to_string(r));
        /// Print numbers.
        for (int w = 0; w < n_weeks; ++w) {
            for (int u = 0; u < water_sources_t.size(); ++u) {
                outStream
                        << setprecision(COLUMN_PRECISION)
                        << water_sources_t[u].available_volume[r][w]
                        << "," << setprecision(COLUMN_PRECISION)
                        << water_sources_t[u].demands[r][w]
                        << "," << setprecision(COLUMN_PRECISION)
                        << water_sources_t[u].total_upstream_sources_inflows[r][w]
                        << "," << setprecision(COLUMN_PRECISION)
                        << water_sources_t[u].total_catchments_inflow[r][w]
                        << "," << setprecision(COLUMN_PRECISION)
                        << water_sources_t[u].evaporated_volume[r][w]
                        << "," << setprecision(COLUMN_PRECISION)
                        << water_sources_t[u].outflows[r][w];
                if (u < water_sources_t.size() - 1)
                    outStream << ",";
            }
            outStream << endl;
        }
        outStream.close();
    }
    cout << "Water sources output printed. Files " << output_directory <<
         file_name << "XX" << endl;
}

void DataCollector::printUtilityOutput(string file_name) {

    std::ofstream outStream;
    int n_weeks = (int) utilities_t[0].st_rof[0].size();
    outStream.open(output_directory + file_name);

    for (int r = 0; r < number_of_realizations; ++r) {

        /// Print realization number.
        outStream << endl;
        outStream << "Realization " << r << endl;
        outStream << endl << setw(COLUMN_WIDTH) << " ";
        for (auto &u : utilities_t) {
            outStream << setw(8 * COLUMN_WIDTH) << u.name;
        }

        /// Print realization header.
        outStream << endl << setw(COLUMN_WIDTH) << "";
        for (int i = 0; i < utilities_t.size(); ++i) {
            outStream << setw(2 * COLUMN_WIDTH) << "Stored"
                      << setw(COLUMN_WIDTH) << " "
                      << setw(COLUMN_WIDTH) << " "
                      << setw(COLUMN_WIDTH) << " "
                      << setw(COLUMN_WIDTH) << "Rest."
                      << setw(COLUMN_WIDTH) << "Unrest."
                      << setw(COLUMN_WIDTH) << "Conting."
                      << setw(COLUMN_WIDTH) << "Insurance"
                      << setw(COLUMN_WIDTH) << "Insurance";
        }
        outStream << endl << setw(COLUMN_WIDTH) << "Week";
        for (int i = 0; i < utilities_t.size(); ++i) {
            outStream << setw(2 * COLUMN_WIDTH) << "Volume"
                      << setw(COLUMN_WIDTH) << "Capacity"
                      << setw(COLUMN_WIDTH) << "ST-ROF"
                      << setw(COLUMN_WIDTH) << "LT-ROF"
                      << setw(COLUMN_WIDTH) << "Demand"
                      << setw(COLUMN_WIDTH) << "Demand"
                      << setw(COLUMN_WIDTH) << "Fund"
                      << setw(COLUMN_WIDTH) << "Payout"
                      << setw(COLUMN_WIDTH) << "Price";
        }
        outStream << endl;

        /// Print numbers for realization r.
        for (int w = 0; w < n_weeks; ++w) {
            outStream << setw(COLUMN_WIDTH) << w;
            for (auto &u : utilities_t) {
                outStream << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.combined_storage[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.capacity[r][w]
                          << setw(COLUMN_WIDTH)
                          << setprecision(COLUMN_PRECISION)
                          << u.st_rof[r][w]
                          << setw(COLUMN_WIDTH)
                          << setprecision(COLUMN_PRECISION)
                          << u.lt_rof[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.restricted_demand[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.unrestricted_demand[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.contingency_fund_size[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.insurance_payout[r][w]
                          << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                          << u.insurance_contract_cost[r][w];
            }
            outStream << endl;
        }

        /// NPC cost of infrastructure for realization r
        for (auto &u : utilities_t) {
            outStream << u.name << " NPC of infrastructure: "
                      << u.net_present_infrastructure_cost[r] << endl;
        }
        outStream << endl;
    }
    outStream.close();

}

void DataCollector::printUtilityOutputCompact(string file_name) {

    int n_weeks = (int) utilities_t[0].st_rof[0].size();

#pragma omp parallel for
    for (int r = 0; r < number_of_realizations; ++r) {
        std::ofstream outStream;
        outStream.open(output_directory + file_name + std::to_string(r));
        outStream << "#";
        for (auto &u : utilities_t) {
            outStream << u.name << " ";
        }

        /// Print numbers for realization r.
        for (int w = 0; w < n_weeks; ++w) {
            for (int u = 0; u < utilities_t.size(); ++u) {
                outStream << utilities_t[u].combined_storage[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].capacity[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].st_rof[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].lt_rof[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].restricted_demand[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].unrestricted_demand[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].contingency_fund_size[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].insurance_payout[r][w]
                          << "," << setprecision(COLUMN_PRECISION)
                          << utilities_t[u].insurance_contract_cost[r][w];
                if (u < utilities_t.size() - 1)
                    outStream << ",";
            }
            outStream << endl;
        }

        /// NPC cost of infrastructure for realization r
        for (int u = 0; u < utilities_t.size(); ++u) {
            outStream << "#" << utilities_t[u].name
                      << " NPC of infrastructure: "
                      << utilities_t[u].net_present_infrastructure_cost[r]
                      << endl;
        }
        outStream << endl;
        outStream.close();
    }
    cout << "Utilities output printed. Files " << output_directory <<
         file_name << "XX" << endl;

}

void DataCollector::printPoliciesOutput(string file_name) {

    std::ofstream outStream;
    outStream.open(output_directory + file_name);

    /// Checks if there were drought mitigation policies in place.
    if (!restriction_policies_t.empty()) {
        int n_weeks = (int) restriction_policies_t[0].restriction_multiplier[0].size();

        sort(restriction_policies_t.begin(), restriction_policies_t.end());

        for (int r = 0; r < number_of_realizations; ++r) {
            if (!restriction_policies_t.empty()) {

                /// Print realization number.
                outStream << endl;
                outStream << "Realization " << r << endl;
                outStream << setw(COLUMN_WIDTH) << " ";

                /// Print realization head line 1.
                for (auto &rp : restriction_policies_t) {
                    outStream << setw(COLUMN_WIDTH - 1) << "Restr. "
                              << rp.utility_id;
                }
                for (auto &rp : transfers_policies_t) {
                    for (int id = 0; id < rp.utilities_ids.size(); ++id)
                        outStream << setw(COLUMN_WIDTH - 1) << "Transf. "
                                  << rp.transfer_policy_id;
                }
                for (auto &rwp : raw_water_transfer_policies_t) {
                    outStream << setw(COLUMN_WIDTH - 1) << "RWT. "
                              << rwp.raw_water_transfer_policy_id;
                    for (int id = 0; id < rwp.utilities_ids.size(); ++id) {
                        outStream << setw(COLUMN_WIDTH - 1) << "RWT. "
                                  << rwp.raw_water_transfer_policy_id;
                    }
                }

                /// Print realization header line 2.
                outStream << endl << setw(COLUMN_WIDTH) << "Week";
                for (int i = 0; i < restriction_policies_t.size(); ++i) {
                    outStream << setw(COLUMN_WIDTH) << "Multip.";
                }
                for (auto &tp : transfers_policies_t) {
                    for (int buyer_id : tp.utilities_ids)
                        outStream << setw(COLUMN_WIDTH - 1) << "Alloc. " << buyer_id;
                }
                // Raw Wat. T. Vol. ID     Raw Wat. T. Rat. Up    Raw Wat. T. Rat. Down
                for (auto &rwp : raw_water_transfer_policies_t) {
                    outStream << setw(COLUMN_WIDTH - 1) << "Vol.";
                    for (int party_id : rwp.utilities_ids) {
                        outStream << setw(COLUMN_WIDTH - 1) << " Rat. " << party_id;
                    }
                }
                outStream << endl;

                /// Print numbers.
                for (unsigned long w = 0; w < n_weeks; ++w) {
                    outStream << setw(COLUMN_WIDTH) << w;
                    for (auto &rp : restriction_policies_t) {
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rp.restriction_multiplier[r].at(w);
                    }
                    for (auto &tp : transfers_policies_t) {
                        for (double &a : tp.demand_offsets[r].at(w))
                            outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << a;
                    }
                    for (auto &rwp : raw_water_transfer_policies_t) {
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rwp.raw_water_transfer_total[r].at(w);
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rwp.utility_target_storage_ratio[r].at(w)[0];
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rwp.utility_target_storage_ratio[r].at(w)[1];
                    }
                    outStream << endl;
                }
            }
        }
    } else {
        outStream << "No restriction policies in place.";
    }
    outStream.close();
}

void DataCollector::printPoliciesOutputCompact(string file_name) {


    /// Checks if there were drought mitigation policies in place.
    if (!restriction_policies_t.empty()) {
        int n_weeks =
                (int) restriction_policies_t[0].restriction_multiplier[0]
                        .size();

        sort(restriction_policies_t.begin(),
             restriction_policies_t.end());

#pragma omp parallel for
        for (int r = 0; r < number_of_realizations; ++r) {
            std::ofstream outStream;
            outStream.open(output_directory + file_name + std::to_string(r));
            if (!restriction_policies_t.empty()) {
                outStream << "#";
                for (auto &rp : restriction_policies_t) {
                    outStream << setw(COLUMN_WIDTH - 1) << "Restr. "
                              << rp.utility_id;
                }
                for (auto &tp : transfers_policies_t) {
                    for (int id = 0;
                         id < tp.utilities_ids.size();
                         ++id)
                        outStream << setw(COLUMN_WIDTH - 1) << "Transf. "
                                  << tp.transfer_policy_id;
                }
                for (auto &rwp : raw_water_transfer_policies_t) {
                    outStream << setw(COLUMN_WIDTH - 1) << "RWT. "
                              << rwp.raw_water_transfer_policy_id;
                    for (int id = 0; id < rwp.utilities_ids.size(); ++id) { // always two parties to raw water transfers
                        outStream << setw(COLUMN_WIDTH - 1) << "RWT. "
                                  << rwp.raw_water_transfer_policy_id;
                    }
                }

                /// Print realization header.
                outStream << endl << "#";
                for (int i = 0; i < restriction_policies_t.size(); ++i) {
                    outStream << setw(COLUMN_WIDTH) << "Multip.";
                }
                for (auto &tp : transfers_policies_t) {
                    for (int buyer_id : tp.utilities_ids)
                        outStream << setw(COLUMN_WIDTH - 1) << "Alloc. "
                                  << buyer_id;
                }
                for (auto &rwp : raw_water_transfer_policies_t) {
                    outStream << setw(COLUMN_WIDTH - 1) << "Vol.";
                    for (int party_id : rwp.utilities_ids) {
                        outStream << setw(COLUMN_WIDTH - 1) << " Rat. " << party_id;
                    }
                }
                outStream << endl;

                /// Print numbers.
                for (unsigned long w = 0; w < n_weeks; ++w) {
//                    outStream << setw(COLUMN_WIDTH) << w;
                    for (auto &rp : restriction_policies_t) {
                        outStream << setprecision(COLUMN_PRECISION)
                                  << rp.restriction_multiplier[r].at(w)
                                  << ",";
                    }
                    unsigned long transfer_size = transfers_policies_t[0].demand_offsets[r].at(w).size();
                    for (auto &rp : transfers_policies_t) {
                        for (int j = 0; j < transfer_size; ++j) {
                            outStream << setprecision(COLUMN_PRECISION)
                                      << rp.demand_offsets[r].at(w)[j] << ",";
//                            if (j < transfer_size - 1)
//                                outStream << ",";
                        }
                    }
                    for (auto &rwp : raw_water_transfer_policies_t) {
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rwp.raw_water_transfer_total[r].at(w) << ",";
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rwp.utility_target_storage_ratio[r].at(w)[0] << ",";
                        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                                  << rwp.utility_target_storage_ratio[r].at(w)[1];
                    }
                    outStream << endl;
                }
            }
            outStream.close();
        }
        cout << "Policies output printed. Files " << output_directory <<
             file_name << "XX" << endl;
    } else {
        std::ofstream outStream;
        outStream.open(output_directory + file_name + std::to_string(-1));
        outStream << "No restriction policies in place.";
    }
}

void DataCollector::printObjectives(string file_name) {

    std::ofstream outStream;
    outStream.open(output_directory + file_name);

    outStream << setw(COLUMN_WIDTH) << "      " << setw((COLUMN_WIDTH * 2)) << "Reliability"
              << setw(COLUMN_WIDTH * 2) << "Restriction Freq."
              //              << setw(COLUMN_WIDTH * 2) << "Jordan Lake Alloc."
              << setw(COLUMN_WIDTH * 2) << "Infrastructure NPC"
              << setw(COLUMN_WIDTH * 2) << "Peak Financial Cost"
              << setw(COLUMN_WIDTH * 2) << "Worse Case Costs" << endl;

    for (Utility_t &ut : utilities_t) {
        outStream << setw(COLUMN_WIDTH) << ut.name;
        for (double o :ut.objectives)
            outStream << setw(COLUMN_WIDTH * 2) <<
                      setprecision(COLUMN_PRECISION) << o;
        outStream << endl;
    }
    outStream.close();
}

/**
 * Calculates all objectives and stores them in the utilities structs.
 */
void DataCollector::calculateObjectives() {

//    /// Sort policies so that the objective values calculated based on them correspond to the right utility.
//    sort(restriction_policies_t.begin(), restriction_policies_t.end());

    int id;
    for (Utility_t &ut : utilities_t) {
        /// Calculate reliability objective.
        ut.objectives.push_back(calculateReliabilityObjective(ut));
        id = ut.id;

        /// Figure out if which utilities have restriction policies in place and calculate restriction frequency
        /// if so. Variable restriction_policy is an instance of vector<RestrictionPolicy_t>::iterator.
        auto restriction_policy = find_if(restriction_policies_t.begin(), restriction_policies_t.end(),
                                          [&id](const RestrictionPolicy_t &rp) { return rp.utility_id == id; });

        if (restriction_policy != restriction_policies_t.end())
            ut.objectives.push_back(calculateRestrictionFrequencyObjective(*restriction_policy));
        else
            ut.objectives.push_back((double &&) NONE);

        /// Calculate NPC of infrastructure objective.
        ut.objectives.push_back(calculateNetPresentCostInfrastructureObjective(ut));
        ut.objectives.push_back(calculatePeakFinancialCostsObjective(ut));
        ut.objectives.push_back(calculateWorseCaseCostsObjective(ut));
    }
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
            for (int w = (int) round(y * WEEKS_IN_YEAR); w < min((int) n_weeks,
                                                                 (int) round((y + 1) * WEEKS_IN_YEAR)); ++w) {
                if (utility.combined_storage[r][w] / utility.capacity[r][w] < STORAGE_CAPACITY_RATIO_FAIL) {
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

    double check_non_zero = accumulate(year_reliabilities.rbegin(), year_reliabilities.rend(), 0.0);

    /// Returns year with most realization failures, divided by the number of realizations (reliability objective).
    if (check_non_zero > 0)
        return 1. - (double) *max_element(year_reliabilities.begin(), year_reliabilities.end()) / n_realizations;
    else
        return 1.;
}

double DataCollector::calculateRestrictionFrequencyObjective(RestrictionPolicy_t restriction_t) {
    unsigned long n_realizations = restriction_t.restriction_multiplier.size();

    /// Check if there were restriction policies in place.
    if (!restriction_t.restriction_multiplier.empty()) {
        unsigned long n_weeks = restriction_t.restriction_multiplier[0].size();
        unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

        vector<vector<double>> year_restriction_frequencies(n_realizations, vector<double>(n_years, 0));
        double restriction_frequency = 0;

        /// Counts how many years across all realizations had restrictions.
        for (int r = 0; r < n_realizations; ++r) {
            for (int y = 0; y < n_years; ++y) {
                for (int w = (int) round(y * WEEKS_IN_YEAR); w < min((int) n_weeks,
                                                                     (int) round((y + 1) * WEEKS_IN_YEAR)); ++w) {
                    if (restriction_t.restriction_multiplier[r][w] != 1.0) {
                        restriction_frequency++;
                        break;
                    }
                }
            }
        }

        return restriction_frequency / (n_realizations * n_years);
    } else
        return NONE;
}

/**
 * Calculate net present cost of new infrastructure objective.
 * @param utility_t
 * @return
 */
double DataCollector::calculateNetPresentCostInfrastructureObjective(Utility_t utility_t) {
    /// Sum net present costs across all realizations.
    double infrastructure_npc = accumulate(utility_t.net_present_infrastructure_cost.begin(),
                                                utility_t.net_present_infrastructure_cost.end(), 0.0);

    return infrastructure_npc / utilities_t.size();
}

/**
 * Calculate peak financial costs objective, as the average of the highest cost
 * year for each realization, with cost * being defined as (DSP + AC + CC ) /
 * TR, where,
 * DSP = debt service payment
 * AC = annual contribution to the contingency fund
 * CC = cost of insurance contract
 * TR = total gross revenue
 * @param utility_t
 * @return objective value
 */
double DataCollector::calculatePeakFinancialCostsObjective(Utility_t utility_t) {
    unsigned long n_realizations = utility_t.gross_revenues.size();
    unsigned long n_weeks = utility_t.gross_revenues[0].size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    double realizations_year_debt_payment = 0;
    double realizations_year_cont_fund_contribution = 0;
    double realizations_year_gross_revenue = 0;
    double realizations_year_insurance_contract_cost = 0;
    vector<double> year_financial_costs;
    vector<double> realization_financial_costs(n_realizations, 0);

    /// Creates a table with years that failed in each realization.
    int y;
    for (int r = 0; r < n_realizations; ++r) {
        year_financial_costs.assign(n_years, 0.0);
        y = 0;
        for (int w = 0; w < n_weeks; ++w) {
            /// accumulate year's info by summing weekly amounts.
            realizations_year_debt_payment +=
                    utility_t.debt_service_payments[r][w];
            realizations_year_cont_fund_contribution +=
                    utility_t.contingency_fund_contribution[r][w];
            realizations_year_gross_revenue +=
                    utility_t.gross_revenues[r][w];
            realizations_year_insurance_contract_cost +=
                    utility_t.insurance_contract_cost[r][w];

            /// if last week of the year, close the books and calculate
            /// financial cost for the year.
            if (Utils::isFirstWeekOfTheYear(w + 1)) {
                year_financial_costs[y] +=
                        (realizations_year_debt_payment +
                         realizations_year_cont_fund_contribution +
                         realizations_year_insurance_contract_cost) /
                        realizations_year_gross_revenue;
                /// update year count.
                y++;

                /// reset accounts.
                realizations_year_debt_payment = 0;
                realizations_year_cont_fund_contribution = 0;
                realizations_year_gross_revenue = 0;
                realizations_year_insurance_contract_cost = 0;
            }
        }
        /// store highest year cost as the cost financial cost of the realization.
        realization_financial_costs[r] =
                *max_element(year_financial_costs.begin(),
                             year_financial_costs.end());
    }

    /// returns average of realizations' costs.
    return accumulate(realization_financial_costs.begin(),
                      realization_financial_costs.end(),
                      0.0) / n_realizations;
}

double DataCollector::calculateWorseCaseCostsObjective(Utility_t utility_t) {
    unsigned long n_realizations = utility_t.gross_revenues.size();
    unsigned long n_weeks = utility_t.gross_revenues[0].size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    vector<double> worse_year_financial_costs(n_realizations, 0);
    vector<double> year_financial_costs;
    double year_drought_mitigation_cost = 0;
    double year_gross_revenue = 0;

    /// Creates a table with years that failed in each realization.
    int y;
    for (int r = 0; r < n_realizations; ++r) {
        y = 0;
        year_financial_costs.assign(n_years, 0);
        for (int w = 0; w < n_weeks; ++w) {
            /// accumulate year's info by summing weekly amounts.
            year_drought_mitigation_cost += utility_t.drought_mitigation_cost[r][w];
            year_gross_revenue += utility_t.gross_revenues[r][w];

            /// if last week of the year, close the books and calculate financial cost for the year.
            if (Utils::isFirstWeekOfTheYear(w + 1)) {
                year_financial_costs[y] = max(year_drought_mitigation_cost -
                                              utility_t.contingency_fund_size[r][w], 0.0)
                                          / year_gross_revenue;

                year_gross_revenue = 0;
                year_drought_mitigation_cost = 0;
                /// update year count.
                y++;
            }
        }
        /// store highest year cost as the drought mitigation cost of the realization.
        worse_year_financial_costs[r] = *max_element(
                year_financial_costs.begin(),
                year_financial_costs.end());
    }

    /// sort costs to get the worse 1 percentile.
    sort(worse_year_financial_costs.begin(), worse_year_financial_costs.end());

    return worse_year_financial_costs.at(
            (unsigned long) floor(WORSE_CASE_COST_PERCENTILE * n_realizations));
}
