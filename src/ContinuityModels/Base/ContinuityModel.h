//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODEL_H
#define TRIANGLEMODEL_CONTINUITYMODEL_H

#include "../../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../../Utils/Constants.h"
#include "../../SystemComponents/Utility.h"
#include "../../Utils/Graph/Graph.h"
#include "../../Controls/Base/MinEnvironFlowControl.h"
#include <vector>

using namespace Constants;
using namespace std;

class ContinuityModel {

protected:

    vector<WaterSource *> continuity_water_sources;
    vector<Utility *> continuity_utilities;
    vector<MinEnvironFlowControl *> min_env_flow_controls;
    Graph water_sources_graph;
    vector<vector<int> > water_sources_to_utilities;
    vector<vector<int> > water_sources_online_to_utilities;
    vector<vector<int> > utilities_to_water_sources;
    vector<int> sources_continuity_order;
    vector<int> downstream_sources;
    vector<int> sources_topological_order;
    vector<double> water_sources_capacities;
    vector<double> utilities_capacities;
    vector<vector<double>> demands;

public:
    const unsigned int realization_id;

    ContinuityModel(
            const vector<WaterSource *> &water_sources,
            const vector<Utility *> &utilities,
            vector<MinEnvironFlowControl *> &min_env_flow_controls,
            const Graph &water_sources_graph,
            const vector<vector<int>> &water_sources_to_utilities,
            unsigned int realization_id);

    ContinuityModel(ContinuityModel &continuity_model);

    void continuityStep(
            int week, int rof_realization = -1, bool
    apply_demand_buffer = false);

    const vector<Utility *> &getUtilities() const;

    virtual ~ContinuityModel();
};


#endif //TRIANGLEMODEL_REGION_H
