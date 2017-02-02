//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODEL_H
#define TRIANGLEMODEL_CONTINUITYMODEL_H

#include "../SystemComponents/WaterSources/WaterSource.h"
#include "../Utils/Constants.h"
#include "../SystemComponents/Utility.h"
#include <vector>

using namespace Constants;
using namespace std;

class ContinuityModel {

protected:
    vector<WaterSource *> water_sources;
    vector<Utility *> utilities;
    vector<vector<int> > water_sources_adjacency_list;
    vector<vector<int> > water_sources_utility_adjacency_list;

public:
    ContinuityModel(const vector<WaterSource *> water_source,
                    const vector<vector<int> > &water_source_connectivity_matrix,
                    const vector<Utility *> utilities,
                    const vector<vector<int> > &water_source_utility_connectivity_matrix);

    void continuityStep(int week, int id_rof = -1);

    const vector<Utility *> &getUtilities() const;
};


#endif //TRIANGLEMODEL_REGION_H
