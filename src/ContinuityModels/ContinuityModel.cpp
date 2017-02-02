//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "ContinuityModel.h"

ContinuityModel::ContinuityModel(const vector<WaterSource *> water_sources,
                                 const vector<vector<int> > &water_sources_connectivity_matrix,
                                 const vector<Utility *> utilities,
                                 const vector<vector<int> > &water_sources_utility_connectivity_matrix) :
        water_sources(water_sources), utilities(utilities) {

    // Assign water_sources to each utility.
    for (int i = 0; i < utilities.size(); i++) {
        for (int j = 0; j < water_sources.size(); j++) {
            if (water_sources_utility_connectivity_matrix[i][j] == 1) {
                this->utilities[i]->addWaterSource(this->water_sources[j]);
            }
        }
    }

    // Convert water source adjacency matrix into adjacency list.
    for (int j = 0; j < water_sources.size(); j++) {
        vector<int> v;
        for (int i = 0; i < water_sources.size(); i++) {
            if (water_sources_connectivity_matrix[i][j] == 1) v.push_back(i);
        }
        if (v.size() == 0) v.push_back(-1);
        water_sources_adjacency_list.push_back(v);
    }

    // Populate water source to utility adjacency list.
    for (int i = 0; i < water_sources.size(); ++i) {
        vector<int> v;
        for (int j = 0; j < utilities.size(); ++j) {
            if (water_sources_utility_connectivity_matrix[j][i] == 1) {
                v.push_back(this->utilities[j]->id);
            }
        }
        water_sources_utility_adjacency_list.push_back(v);
    }

    // Update all utilities' combined storage volume.
    for (Utility *u : this->utilities) {
        u->updateTotalStoredVolume();
    }

/*/    // Print water source adjacency list. FOR TESTS ONLY.
//    cout << endl << "Water source connections." << endl;
//    for (int k = 0; k < water_sources_adjacency_list.size(); ++k) {
//        vector<int> row = water_sources_adjacency_list[k];
//        cout << water_sources[k]->source_type << " " << k << ": ";
//        for (int &i : row) {
//            cout << i << " ";
//        }
//        cout << endl;
//    }
//
//
//    // Print list of reservoir for each utility. FOR TESTS ONLY.
//    cout << endl << "Water sources belonging to each utility." << endl;
//    for (Utility *u : this->utilities) {
//        cout << "Utility " << u->id << ": ";
//        for (const map<int, WaterSource *>::value_type &r : u->getWaterSource()) {
//            cout << r.second->id << " ";
//        }
//        cout << endl;
//    }
//
//    cout << endl;
//
//    // Print list of utilities drawing from each water source. FOR TESTS ONLY.
//    cout << endl << "Utilities drawing from water source." << endl;
//    for (int i = 0; i < water_sources_utility_adjacency_list.size(); ++i) {
//        cout << water_sources[i]->source_type << " " << i << ": ";
//        for (int &id : water_sources_utility_adjacency_list[i]) {
//            cout << id << " ";
//        }
//        cout << endl;
//    }
//    cout << endl;
*/
}

/**
 * Calculates continuity for one week time step for streamflows of id_rof years before current week.
 * @param week current week.
 * @param id_rof rof realization id (between 0 and 49 inclusive).
 */
void ContinuityModel::continuityStep(int week, int id_rof) {
    double demands[water_sources.size()] = {};
    double upstream_releases_inflows[water_sources.size()] = {};

    /*
     * Split weekly demands among each reservoir for each utility. For each water source:
     * (1) sums the demands of each drawing utility to come up with the total demand_previous_week for
     * that week for that water source, and (2) sums the flow contributions of upstream
     * reservoirs.
     */

    for (Utility *u : utilities) {
        u->splitDemands(week);
    }

    for (int i = 0; i < water_sources.size(); ++i) {
        // gets demands from utilities
        for (int &j : water_sources_utility_adjacency_list[i]) {
            demands[i] += utilities[j]->getDemand(i);
        }
        // gets flows from upstream sources.
        for (int &j : water_sources_adjacency_list[i]) {
            if (j > -1) {
                upstream_releases_inflows[i] += water_sources[j]->getOutflow_previous_week();
            }
        }
    }

    /*
     * For all water sources, performs mass balance to update the available volume. The week here is
     * shifted back according to the rof year realization (0 to 49, inclusive) so that the right flows are
     * gotten from source catchments for each rof year realization.
     */
    for (int i = 0; i < water_sources.size(); i++) {
        water_sources[i]->updateAvailableVolume(week - (id_rof + 1) * WEEKS_IN_YEAR,
                                                upstream_releases_inflows[i], demands[i]);

//        if (id_rof < 0) cout << this->water_sources[i]->getAvailable_volume() <<
//             " " << this->water_sources[i]->getOutflow_previous_week() << " " << demands[i] << " ";
    }
//    if (id_rof < 0) cout << "" << endl;

    // updates combined storage for utilities.
    for (Utility *u : utilities) {
        u->updateTotalStoredVolume();
    }
}

const vector<Utility *> &ContinuityModel::getUtilities() const {
    return utilities;
}




