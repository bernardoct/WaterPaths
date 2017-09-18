//
// Created by David on 9/18/2017.
//

#ifndef TRIANGLEMODEL_RAWWATERRELEASEDATACOLLECTOR_H
#define TRIANGLEMODEL_RAWWATERRELEASEDATACOLLECTOR_H

#include <vector>
#include "Base/DataCollector.h"
#include "../DroughtMitigationInstruments/RawWaterReleases.h"

class RawWaterReleaseDataCollector : public DataCollector {
private:
    vector<int> utilities_ids;
    vector<vector<double>> storage_targets;
    double raw_water_volume_released;
    RawWaterReleases *raw_water_releases_policy;
    int N_COLUMNS = 1;

public:
    RawWaterReleaseDataCollector(RawWaterReleases *raw_water_releases_policy);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;

};

#endif //TRIANGLEMODEL_RAWWATERRELEASEDATACOLLECTOR_H
