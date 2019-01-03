//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "ContinuityModel.h"
#include "../../SystemComponents/WaterSources/SequentialJointTreatmentExpansion.h"

ContinuityModel::ContinuityModel(vector<WaterSource *> &water_sources, vector<Utility *> &utilities,
                                 vector<MinEnvFlowControl *> &min_env_flow_controls,
                                 const Graph &water_sources_graph,
                                 const vector<vector<int>> &water_sources_to_utilities,
                                 vector<double> &utilities_rdm,
                                 vector<double> &water_sources_rdm,
                                 unsigned long realization_id) :
        continuity_water_sources(water_sources),
        continuity_utilities(utilities),
        min_env_flow_controls(min_env_flow_controls),
        water_sources_graph(water_sources_graph),
        water_sources_to_utilities(water_sources_to_utilities),
        sources_topological_order(water_sources_graph.getTopological_order()), /// Get topological order so that mass balance is ran from up to downstream.
        utilities_rdm(utilities_rdm),
        water_sources_rdm(water_sources_rdm),
        n_utilities((int) utilities.size()),
        n_sources((int) water_sources.size()),
        realization_id(realization_id)
        {

    //FIXME: THERE IS A STUPID MISTAKE HERE IN THE SORT FUNCTION THAT IS PREVENTING IT FROM WORKING UNDER WINDOWS AND LINUX.
    std::sort(continuity_water_sources.begin(), continuity_water_sources.end(), WaterSource::compare);
#ifdef _WIN32
    sort(continuity_utilities.begin(), continuity_utilities.end(), std::greater<>());
#else
    std::sort(continuity_utilities.begin(), continuity_utilities.end(), Utility::compById);
#endif

    checkInput(water_sources, utilities, min_env_flow_controls,
            water_sources_graph, water_sources_to_utilities,
            utilities_rdm, water_sources_rdm, realization_id);

    // Link water sources to utilities by passing pointers of the former to
    // the latter.
    for (unsigned long u = 0; u < utilities.size(); ++u) {
        for (unsigned long ws = 0; ws < water_sources_to_utilities[u].size(); ++ws) {
            WaterSource *water_source =
                    continuity_water_sources[water_sources_to_utilities[u][ws]];
            this->continuity_utilities[u]->addWaterSource(water_source);
        }
    }

    // Create table showing which utilities draw water from each water source.
    utilities_to_water_sources.assign(water_sources.size(), vector<int>(0));
    water_sources_online_to_utilities.assign(water_sources.size(), vector<int>(0));
    for (unsigned long u = 0; u < utilities.size(); ++u) {
        for (const int &ws : water_sources_to_utilities[u]) {
            utilities_to_water_sources[ws].push_back(u);
            if (water_sources[ws]->isOnline())
                water_sources_online_to_utilities[u].push_back(ws);
        }
    }

    /// The variables below are to make the storage-ROF table calculation
    /// faster by limiting the storage curve shifting to online water sources.
    for (auto water_source : water_sources) {
        bool online = false;

        for (unsigned long u = 0; u < utilities.size(); ++u)
            if (water_source->isOnline())
                online = true;

        if (online)
            water_sources_capacities.push_back(
                    water_source->getSupplyCapacity());
        else
            water_sources_capacities.push_back((double) NONE);
    }

    for (Utility *u : continuity_utilities)
        utilities_capacities.push_back(u->getTotal_storage_capacity());

    /// Populate vector indicating the downstream source from each source.
    for (vector<int> ds : water_sources_graph.getDownSources())
        if (ds.empty())
            downstream_sources.push_back(NON_INITIALIZED);
        else
            downstream_sources.push_back(ds[0]);

    /// Add reference to water sources and utilities so that controls can
    /// access their info.
    for (MinEnvFlowControl *mef : this->min_env_flow_controls)
        mef->addComponents(water_sources, utilities);

    /// Set realization id on utilities and water sources, so that they use the
    /// right streamflow, evaporation and demand data.
    setRealization(realization_id, utilities_rdm, water_sources_rdm);

    demands = std::vector<vector<double>>(
            continuity_water_sources.size(),
            vector<double>(continuity_utilities.size(), 0.));

    /// populate array delta_realization_weeks so that the rounding and casting don't
    /// have to be done every time continuityStep is called, avoiding a bottleneck.
    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        delta_realization_weeks[r] = (int) std::round((r + 1) * WEEKS_IN_YEAR);
    }
}

ContinuityModel::~ContinuityModel() {
    /// Delete water sources
    for (auto ws : continuity_water_sources) {
        delete ws;
    }

    /// Delete utilities
    for (auto u : continuity_utilities) {
        delete u;
    }

    /// Delete min env flow controls
    for (auto mef : min_env_flow_controls){
        delete mef;
    }
}

void ContinuityModel::checkInput(vector<WaterSource *> &water_sources, vector<Utility *> &utilities,
                                 vector<MinEnvFlowControl *> &min_env_flow_controls,
                                 const Graph &water_sources_graph,
                                 const vector<vector<int>> &water_sources_to_utilities,
                                 vector<double> &utilities_rdm,
                                 vector<double> &water_sources_rdm,
                                 unsigned long realization_id) {

//    for (auto ws : water_sources) {
//        auto number_of_utilities_in_ws = ws->getAllocated_treatment_fractions().size();
//        if (number_of_utilities_in_ws > 0 &&
//            ws->getUtilities_with_allocations()->back() == ws->getWq_pool_id()) {
//            --number_of_utilities_in_ws;
//        }
//        if (number_of_utilities_in_ws > 1 &&
//                ws->getAllocated_treatment_fractions().size() != utilities.size()) {
//            string error = "Number of utilities with allocated treatment in "
//                           "water source ";
//            error += to_string(ws->id) + " is different than the number of "
//                                         "utilities in the system.";
//            __throw_invalid_argument(error.c_str());
//        }
//    }

}

/**
 * Calculates continuity for one week time step for streamflows of id_rof years
 * before current week.
 * @param week current week.
 * @param rof_realization rof realization id (between 0 and 49 inclusive).
 */
void ContinuityModel::continuityStep(
        int week, int rof_realization, bool apply_demand_buffer) {
    double* upstream_spillage = new double[n_sources];
    fill_n(upstream_spillage, n_sources, 0.);
    double* wastewater_discharges = new double[n_sources];
    fill_n(wastewater_discharges, n_sources, 0.);

    /**
     * Get wastewater discharges based on previous week's demand.
     *
     * Split weekly demands among each reservoir for each utility. For each
     * water source: (1) sums the demands of each drawing utility to come up
     * with the total unrestricted_demand for that week for that water
     * source, and (2) sums the flow contributions of upstream reservoirs.
     */
    for (Utility *u : continuity_utilities) {
        u->calculateWastewater_releases(week, wastewater_discharges);
        u->splitDemands(week, demands, apply_demand_buffer);
    }

    /**
     * Set minimum environmental flows for water sources based on their
     * individual controls.
     */
    for (MinEnvFlowControl *c : min_env_flow_controls) {
        continuity_water_sources[c->water_source_id]->
                setMin_environmental_outflow(c->getRelease(week));
    }

    /**
     * For all water sources, performs mass balance to update the available
     * volume. The week here is shifted back according to the rof year
     * realization (0 to 49, inclusive) so that the right flows are gotten
     * from source catchments for each rof year realization. If this is not an
     * rof calculation but an actual simulation instead, rof_realization will
     * be equal to -1 (see header file) so that there is no week shift.
     */
    auto& upstream_sources_ids = water_sources_graph.getUpstream_sources();
    for (int i : sources_topological_order) {
        /// Sum spillage from all sources upstream source i.
        for (int ws : upstream_sources_ids[i]) {
            upstream_spillage[i] += continuity_water_sources.at(
                            static_cast<unsigned long>(ws))->getTotal_outflow();
        }

        /// Apply
        continuity_water_sources[i]->continuityWaterSource(
                week - delta_realization_weeks[rof_realization],
                upstream_spillage[i], wastewater_discharges[i], demands[i]);
    }

    /// updates combined storage for utilities.
    for (Utility *u : continuity_utilities) {
        u->updateTotalAvailableVolume();
    }

    delete[] upstream_spillage;
    delete[] wastewater_discharges;
}

void ContinuityModel::setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                                     vector<double> &water_sources_rdm) {
    if (realization_id != (unsigned) NON_INITIALIZED) {
        for (Utility *u : continuity_utilities)
            u->setRealization(realization_id, utilities_rdm);
        for (WaterSource *ws : continuity_water_sources)
            ws->setRealization(realization_id, water_sources_rdm);
        for (MinEnvFlowControl *mef : min_env_flow_controls)
            mef->setRealization(realization_id, water_sources_rdm);
    }
}

/*
 * Get list of next online downstream sources for each source.
 * @return vector with downstream water sources.
 */
vector<int> ContinuityModel::getOnlineDownstreamSources() {
    vector<int> online_downstream_sources(n_sources, NON_INITIALIZED);
    for (int ws : sources_topological_order) {
        int downstream_source_online = ws;
        do {
            downstream_source_online = downstream_sources[downstream_source_online];
        } while (downstream_source_online != NON_INITIALIZED &&
                !continuity_water_sources[downstream_source_online]->isOnline() &&
                continuity_water_sources[ws]->getSupplyCapacity() == 0);
        online_downstream_sources[ws] = downstream_source_online;
    }

    return online_downstream_sources;
}

const vector<WaterSource *> &ContinuityModel::getContinuity_water_sources() const {
    return continuity_water_sources;
}

const vector<Utility *> &ContinuityModel::getContinuity_utilities() const {
    return continuity_utilities;
}
