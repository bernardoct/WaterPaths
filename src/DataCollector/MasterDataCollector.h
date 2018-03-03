//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_MASTERDATACOLLECTOR_H
#define TRIANGLEMODEL_MASTERDATACOLLECTOR_H


#include <vector>
#include "Base/DataCollector.h"
#include "UtilitiesDataCollector.h"
#include "../DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h"

class MasterDataCollector {
private:
    string output_directory;

    vector<vector<DataCollector *>> water_source_collectors;
    vector<vector<DataCollector *>> drought_mitigation_policy_collectors;
    vector<vector<UtilitiesDataCollector>> utility_collectors;

public:



    vector<double> calculatePrintObjectives(string file_name, bool print);

    virtual ~MasterDataCollector();

    void printPoliciesOutputCompact(
            int week_i, int week_f, string file_name);

    void printPoliciesOutputTabular(int week_i, int week_f, string file_name);

    void printUtilitiesOutputCompact(int week_i, int week_f, string file_name);

    void printUtilitesOutputTabular(int week_i, int week_f, string file_name);

    void
    printWaterSourcesOutputCompact(int week_i, int week_f, string file_name);

    void
    printWaterSourcesOutputTabular(int week_i, int week_f, string file_name);

    void setOutputDirectory(string directory);

    void printPathways(string file_name);

    void addRealization(
            vector<WaterSource *> water_sources_realization,
            vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization,
            vector<Utility *> utilities_realization, unsigned long r);

    void collectData(unsigned long r);
};


#endif //TRIANGLEMODEL_MASTERDATACOLLECTOR_H
