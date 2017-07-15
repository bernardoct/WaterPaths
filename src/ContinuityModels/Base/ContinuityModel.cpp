//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include <cmath>
#include "ContinuityModel.h"

ContinuityModel::ContinuityModel(
        const vector<WaterSource *> &water_sources,
        const vector<Utility *> &utilities,
        vector<MinEnvironFlowControl *> &min_env_flow_controls,
        const Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        unsigned int realization_id) :
        continuity_water_sources(water_sources),
        continuity_utilities(utilities),
        min_env_flow_controls(min_env_flow_controls),
        water_sources_graph(water_sources_graph),
        water_sources_to_utilities(water_sources_to_utilities),
        realization_id(realization_id) {

    /// Connect water sources to utilities.
    for (int i = 0; i < utilities.size(); ++i) {
        for (int j = 0; j < water_sources_to_utilities[i].size(); ++j) {
            this->continuity_utilities[i]->addWaterSource(
                    water_sources[water_sources_to_utilities[i][j]]);
        }
    }

    utilities_to_water_sources.assign(water_sources.size(), vector<int>(0));
    for (int i = 0; i < utilities.size(); ++i) {
        for (const int &j : water_sources_to_utilities[i]) {
            utilities_to_water_sources[j].push_back(i);
        }
    }

    /// Get topological order so that mass balance is ran from up to downstream.
    sources_continuity_order = water_sources_graph.getTopological_order();

    /// The variables below are to make the storage-ROF table calculation
    /// faster.
    //FIXME: DO I NEED THIS FOR LOOP BELOW?
    for (int ws = 0; ws < water_sources.size(); ++ws) {
        if (water_sources[ws]->isOnline())
            water_sources_capacities.push_back(
                    water_sources[ws]->getCapacity());
        else
            water_sources_capacities.push_back((double &&) NONE);
    }

    for (Utility *u : continuity_utilities)
        utilities_capacities.push_back(u->getTotal_storage_capacity());

    for (vector<int> ds : water_sources_graph.getDownSources())
        if (ds.empty())
            downstream_sources.push_back(NON_INITIALIZED);
        else
            downstream_sources.push_back(ds[0]);

    sources_topological_order = water_sources_graph.getTopological_order();

    /// Set realization id on utilities and water sources, so that they use the
    /// right streamflow and demand data.
    for (Utility *u : continuity_utilities)
        u->setRelization(realization_id);
    for (WaterSource *ws : continuity_water_sources)
        ws->setRealization(realization_id);
    for (MinEnvironFlowControl *mef : this->min_env_flow_controls)
        mef->setRealization(realization_id);

    /// Add reference to water sources and utilities so that controls can
    /// access their info.
    for (MinEnvironFlowControl *mef : this->min_env_flow_controls)
        mef->addComponents(water_sources, utilities);

}

ContinuityModel::~ContinuityModel() {}

ContinuityModel::ContinuityModel(ContinuityModel &continuity_model) :
        realization_id(continuity_model.realization_id) {}

/**
 * Calculates continuity for one week time step for streamflows of id_rof years
 * before current week.
 * @param week current week.
 * @param rof_realization rof realization id (between 0 and 49 inclusive).
 */
void ContinuityModel::continuityStep(int week, int rof_realization) {
//    auto *demands = new double[continuity_water_sources.size()]();
    auto *demands = new std::vector<vector<double>>
            (continuity_water_sources.size(),
             vector<double>(continuity_utilities.size(),
                            0));
    auto *upstream_spillage = new double[continuity_water_sources.size()]();
    auto *wastewater_discharges =
            new double[continuity_water_sources.size()]();

    /**
     * Get wastewater discharges based on previous week's demand.
     *
     * Split weekly demands among each reservoir for each utility. For each
     * water source: (1) sums the demands of each drawing utility to come up
     * with the total unrestricted_demand for that week for that water
     * source, and (2) sums the flow contributions of upstream reservoirs.
     */
    for (Utility *u : continuity_utilities) {
        u->calculateWastewater_releases(week,
                                        wastewater_discharges);
        u->splitDemands(week,
                        demands); //FIXME: MAKE IT POSSIBLE TO HAVE AN
        // ARRAY OF DEMANDS BEING PASSED TO THE SPLITDEMANDS FUNCTION TO MAKE IT WORK WITH RESERVOIR WITH ALLOCATIONS.
    }

    /**
     * Set minimum environmental flows for water sources based on their
     * individual controls.
     */
    for (MinEnvironFlowControl *c : min_env_flow_controls) {
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
    for (int &i : sources_continuity_order) {
        for (int &ws : water_sources_graph.getUpstream_sources(i))
            upstream_spillage[i] +=
                    continuity_water_sources[ws]->getTotal_outflow();

        continuity_water_sources[i]->continuityWaterSource(
                week - (int) std::round((rof_realization + 1) * WEEKS_IN_YEAR),
                upstream_spillage[i] + wastewater_discharges[i],
                &(*demands)[i]);
    }

    /// updates combined storage for utilities.
    for (Utility *u : continuity_utilities) {
        u->updateTotalStoredVolume();
    }

    delete demands;
    delete[] upstream_spillage;
    delete[](wastewater_discharges);
}

const vector<Utility *> &ContinuityModel::getUtilities() const {
    return continuity_utilities;
}




