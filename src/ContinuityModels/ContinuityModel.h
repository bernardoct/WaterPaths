//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODEL_H
#define TRIANGLEMODEL_CONTINUITYMODEL_H

#include "../SystemComponents/WaterSources/WaterSource.h"
#include "../Utils/Constants.h"
#include "../SystemComponents/Utility.h"
#include "../Utils/Graph/Graph.h"
#include <vector>

using namespace Constants;
using namespace std;

class ContinuityModel {

protected:
    vector<WaterSource *> continuity_water_sources;
    vector<Utility *> continuity_utilities;
    Graph water_sources_graph;
    vector<vector<int> > water_sources_to_utilities;
    vector<vector<int> > utilities_to_water_sources;
    vector<int> reservoir_continuity_order;

public:
    ContinuityModel(const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
                    const Graph &water_sources_graph, const vector<vector<int>> &water_sources_to_utilities);

    void continuityStep(int week, int rof_realization = -1);

    const vector<Utility *> &getUtilities() const;

    virtual ~ContinuityModel();
};


#endif //TRIANGLEMODEL_REGION_H
