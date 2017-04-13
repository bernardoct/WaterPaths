//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "ContinuityModel.h"
#include "../Utils/Graph/WaterSourcesGraph.h"


ContinuityModel::ContinuityModel(const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
                                 const WaterSourceGraph &water_sources_graph,
                                 const vector<vector<int>> &water_sources_to_utilities) : water_sources(water_sources),
                                                                                          utilities(utilities),
                                                                                          water_sources_graph(
                                                                                                  water_sources_graph),
                                                                                          water_sources_to_utilities(
                                                                                                  water_sources_to_utilities) {
    /// Connect water sources to utilities.
    for (int i = 0; i < utilities.size(); ++i) {
        for (int j = 0; j < water_sources_to_utilities[i].size(); ++j) {
            this->utilities.at((unsigned long) i)->addWaterSource(
                    water_sources.at((unsigned long) water_sources_to_utilities[i][j]));
        }
    }

    utilities_to_water_sources.assign(water_sources.size(), vector<int>(0));
    for (int i = 0; i < utilities.size(); ++i) {
        for (const int &j : water_sources_to_utilities.at((unsigned long) i)) {
            utilities_to_water_sources.at((unsigned long) j).push_back(i);
        }
    }

    /// Update all utilities' combined storage volume.
    for (Utility *u : this->utilities) {
        u->updateTotalStoredVolume();
    }

    reservoir_continuity_order = water_sources_graph.getTopological_order();
}

/**
 * Calculates continuity for one week time step for streamflows of id_rof years before current week.
 * @param week current week.
 * @param id_rof rof realization id (between 0 and 49 inclusive).
 */
void ContinuityModel::continuityStep(int week, int id_rof) {
    double demands[water_sources.size()] = {};
    double upstream_spillage[water_sources.size()] = {};

    /*
     * Split weekly demands among each reservoir for each utility. For each water source:
     * (1) sums the demands of each drawing utility to come up with the total unrestricted_demand for
     * that week for that water source, and (2) sums the flow contributions of upstream
     * reservoirs.
     */
    for (Utility *u : utilities) {
        u->splitDemands(week);
    }

    for (int j = 0; j < water_sources.size(); ++j) {
        // gets demands from utilities
        for (int &i : utilities_to_water_sources[j]) {
            demands[j] += utilities.at((unsigned long) i)->getReservoirDraw(j);
        }
    }

    /*
     * For all water sources, performs mass balance to update the available volume. The week here is
     * shifted back according to the rof year realization (0 to 49, inclusive) so that the right flows are
     * gotten from source catchments for each rof year realization.
     */
    for (int &i : reservoir_continuity_order) {
        for (int &ws : water_sources_graph.getUpstream_sources(i))
            upstream_spillage[i] += water_sources[ws]->getTotal_outflow();


        water_sources[i]->continuityWaterSource(week - (id_rof + 1) * WEEKS_IN_YEAR,
                                                upstream_spillage[i], demands[i]);
    }

    // updates combined storage for utilities.
    for (Utility *u : utilities) {
        u->updateTotalStoredVolume();
    }
}

const vector<Utility *> &ContinuityModel::getUtilities() const {
    return utilities;
}




