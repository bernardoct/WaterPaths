# Parsing Your Own Class

Input file parsing in WaterPaths is managed by the MasterSystemInputFileParser class. Its method parseFile reads the header of the input file containing the number of realizations and weeks to be simulated and parses all elements describing the system . The data pertaining to each element, be it a water source, utility, or a connectivity graph, is demarcated with a tag following the format "\[MY TAG\]". Right below tag, the data is organized one parameter and its data per line, as in "paramter_name text_string 0 1 2,3,4". In this example, the line is comprised of the name of the parameter (preferably corresponding to the name of the variable in the code of the parser itself), a string variable, two doubles or integers, and a vector or integers. More details about how to use already implemented code, how to organize your parser, and about how to integrate it within WaterPaths parser are given below.  

## Water Sources

WaterPaths does the parsing of several basic variables common to all water sources, such as name and capacity, and has a number of auxiliary functions and exceptions implemented. Here is how to make the best use of them and add a parser to your own water source to WaterPaths.

1.  All water sources parsers are to publicly extend the WaterSourceParser class, located in [src/InputFileParser/Base/WaterSourceParser.h](https://github.com/bernardoct/WaterPaths/tree/inputFileParser/src/InputFileParser/Base). 
2. Variables for all variables that may be used in any constructor of the relevant water source must be declared in the header file for persistence and organization.
3. The parsing of the data from the *block* variable is to be centralized and performed in the method *parseVariables(vector\<vector\<string\>\> &block, int line_no, int n_realizations, int n_weeks)*. The *parseVariables* loops through *block* and stores in class variables declared in the header copies of its strings converted to the appropriate formats---useful functions for the conversion of strings to doubles, ints and vectors are *stoi(string var)*, *stod(string var)*, and *Utils::tokenizeString*. After a line (vector\<string\> in block) is read, its index is to be stored in a vector, say *vector<unsigned long> rows_read*. Lastly, the function *cleanBlock(block, rows_read);* must be called at the very bottom of *parseVariables* so that the read parameters are deleted from the block to allow for later checking for unread parameters. A call to the *function WaterSourceParser::parseVariables* can be added to the beginning of the new *MyNewParser::parseVariables* function to avoid coding the parsing of common variables such as name, bonds, etc. Below is an example of the parseVariables function:

```cpp
void NewClassParser::parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks) {
    WaterSourceParser::parseVariables(block, n_realizations, n_weeks);

    vector<unsigned long> rows_read;

    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string>& line = block[i];
        if (line[0] == "streamflow_files") {
            catchment_parser.parseSeries(
                    vector<string>(line.begin() + 1, line.end()),
                    n_weeks, n_realizations
                    );
            catchments = catchment_parser.getParsedCatchments();
            rows_read.push_back(i);
        } else if (line[0] == "treatment_capacity") {
            treatment_capacity = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "allocation_ids") {
            Utils::tokenizeString(line[1], allocation_ids, ',');
            rows_read.push_back(i); 
        }
    }

    cleanBlock(block, rows_read);
}
```

4. The new parser must implement the pure virtual method *generateSource(int id, vector\<vector\<string\>\> &block, int line_no, int n_realizations, int n_weeks)*, which is the method that returns the pointer to the newly created water source object. This method is to create a water source object from the received id and block variable, containing all parameters and corresponding data. The *line_no* variable is used mostly for throwing exceptions that show the number of the line where the problem is and the number of realizations and weeks are used to parse csv files, if needed. The method *parseVariables* is to be called at the beginning of the *generateSource* method followed by the *checkMissingOrExtraParams(line_no, block);* method to check for parameters not read and for inconsistencies in mutually implicative parameters (e.g. a bond interest rate is provided but not a bond term). Different constructors for the new source can be called depending on which variables were parsed, as in the example below:

```cpp
WaterSource *ReservoirParser::generateSource(int id, vector<vector<string>> &block, int line_no, int n_realizations,
                                             int n_weeks) {

    parseVariables(block, n_realizations, n_weeks);
    checkMissingOrExtraParams(line_no, block);

    if (existing_infrastructure && variable_area) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series,
                             storage_vs_area_curves);
    } else if (existing_infrastructure) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series, storage_area);
    } else if (!existing_infrastructure && variable_area) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series,
                  storage_vs_area_curves, construction_time,
                             permitting_time, *bonds.back());
    } else if (!existing_infrastructure) {
        return new Reservoir(name, id, catchments, capacity,
                             treatment_capacity, evaporation_series, storage_area, construction_time,
                  permitting_time, *bonds.back());
    } else {
        throw invalid_argument("Really weird error in ReservoirParser. Please "
                               "report this to bct52@cornell.edu");
    }
}
```
5. Sometimes blocks will have variables that need to be preprocessed before they can be parsed and input into a constructor. Examples are names of water sources and utilities being replaced by their IDs, which are assigned by the master parser without user's knowledge. To preprocess a block, the user is advised to create a *preProcessBlock* method taking as parameters, for example, block, tag_name, line_no, ws_name_to_id. The replacement of name by ID can be performed by means of the function *AuxDataParser::replaceNameById* method. In the example below, *replacementNameById* looks into the block of a reservoir expansion for the parameter "parent_reservoir" and replaces its first ("1") value, which is here the name of a reservoir, by the corresponding reservoir id. 
```cpp
WaterSource *
ReservoirExpansionParser::generateSource(int id, vector<vector<string>> &block,
                                         int line_no, int n_realizations,
                                         int n_weeks,
                                         const map<string, int> &ws_name_to_id,
                                         const map<string, int> &utility_name_to_id) {
    preProcessBlock(block, tag_name, line_no, ws_name_to_id);
    parseVariables(block, n_realizations, n_weeks);
    checkMissingOrExtraParams(line_no, block);

    return new ReservoirExpansion(name, id, parent_reservoir_ID, capacity,
                                  construction_time, permitting_time,
                                  *bonds[0]);
}

void ReservoirExpansionParser::checkMissingOrExtraParams(int line_no,
                                                         vector<vector<string>> &block) {
    WaterSourceParser::checkMissingOrExtraParams(line_no, block);
}
```
6. The next step is to add a tag/block reader to the function *MasterSystemInputFileParser::createWaterSource* in [MasterSystemInputFileParser.cpp](https://github.com/bernardoct/WaterPaths/blob/inputFileParser/src/InputFileParser/MasterSystemInputFileParser.cpp). The block checks for the appropriate tag, creates the data parser, and uses it to create a pointer to the newly created object and store it in the *water_sources* vector. An example is shown below:
```cpp
   if (tag == "[RESERVOIR EXPANSION]") {
        water_source_parsers.push_back(new ReservoirExpansionParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1]
                        )
        );
   } else if ...
```

## Drought Mitigation Policies

