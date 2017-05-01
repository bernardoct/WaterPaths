//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include <cmath>
#include "ContinuityModel.h"


ContinuityModel::ContinuityModel(const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
                                 const Graph &water_sources_graph,
                                 const vector<vector<int>> &water_sources_to_utilities) : continuity_water_sources(
        water_sources),
                                                                                          continuity_utilities(
                                                                                                  utilities),
                                                                                          water_sources_graph(
                                                                                                  water_sources_graph),
                                                                                          water_sources_to_utilities(
                                                                                                  water_sources_to_utilities) {
    /// Connect water sources to utilities.
    for (int i = 0; i < utilities.size(); ++i) {
        for (int j = 0; j < water_sources_to_utilities[i].size(); ++j) {
            this->continuity_utilities.at((unsigned long) i)->addWaterSource(
                    water_sources.at((unsigned long) water_sources_to_utilities[i][j]));
        }
    }

    utilities_to_water_sources.assign(water_sources.size(), vector<int>(0));
    for (int i = 0; i < utilities.size(); ++i) {
        for (const int &j : water_sources_to_utilities.at((unsigned long) i)) {
            utilities_to_water_sources.at((unsigned long) j).push_back(i);
        }
    }

    /// Get topological order so that mass balance is ran from up to downstream.
    reservoir_continuity_order = water_sources_graph.getTopological_order();

    water_sources_draws = new vector<double>(water_sources.size());
}

ContinuityModel::~ContinuityModel() {

}

ContinuityModel::ContinuityModel(ContinuityModel &continuity_model) {}

/**
 * Calculates continuity for one week time step for streamflows of id_rof years before current week.
 * @param week current week.
 * @param rof_realization rof realization id (between 0 and 49 inclusive).
 */
void ContinuityModel::continuityStep(int week, int rof_realization) {
    double demands[continuity_water_sources.size()] = {};
    double upstream_spillage[continuity_water_sources.size()] = {};

    /**
     * Split weekly demands among each reservoir for each utility. For each water source:
     * (1) sums the demands of each drawing utility to come up with the total unrestricted_demand for
     * that week for that water source, and (2) sums the flow contributions of upstream
     * reservoirs.
     */
    for (Utility *u : continuity_utilities) {
        u->splitDemands(week, water_sources_draws);
    }

    for (int j = 0; j < continuity_water_sources.size(); ++j) {
        /// gets demands from utilities
        for (int i : utilities_to_water_sources[j]) {
            demands[j] += (*water_sources_draws)[i];//continuity_utilities[i]->getReservoirDraw(j);
        }
    }

    /**
     * For all water sources, performs mass balance to update the available volume. The week here is
     * shifted back according to the rof year realization (0 to 49, inclusive) so that the right flows are
     * gotten from source catchments for each rof year realization. If this is not an rof calculation but an actual
     * simulation instead, rof_realization will be equal to -1 (see header file) so that there is no week shift.
     */
    for (int &i : reservoir_continuity_order) {
        for (int &ws : water_sources_graph.getUpstream_sources(i))
            upstream_spillage[i] += continuity_water_sources[ws]->getTotal_outflow();

        continuity_water_sources[i]->continuityWaterSource(
                week - (int) std::round((rof_realization + 1) * WEEKS_IN_YEAR),
                upstream_spillage[i], demands[i]);
    }

    /// updates combined storage for utilities.
    for (Utility *u : continuity_utilities) {
        u->updateTotalStoredVolume();
    }
    water_sources_draws->clear();
}

const vector<Utility *> &ContinuityModel::getUtilities() const {
    return continuity_utilities;
}




