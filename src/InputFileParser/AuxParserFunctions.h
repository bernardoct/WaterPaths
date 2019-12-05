//
// Created by Bernardo on 11/29/2019.
//

#ifndef TRIANGLEMODEL_AUXPARSERFUNCTIONS_H
#define TRIANGLEMODEL_AUXPARSERFUNCTIONS_H

#include <vector>
#include <string>
#include <algorithm>
#include "../Utils/Graph/Graph.h"

using namespace std;

class AuxParserFunctions {
public:
    /**
     * Checks if there are tags left in block, meaning the were not parsed
     * likely because of a typo or other mistakes.
     * @param line_no
     * @param block list of parameter and their values for that system component.
     * @param tag_name
     */
    static void
    checkForUnreadTags(int line_no, const vector <vector<string>> &block,
                       const string &tag_name);

    /**
     * Removes from block read parameters and their values.
     * @param block
     * @param rows_read block rows containing parameters successfully parsed.
     */
    static void
    cleanBlock(vector <vector<string>> &block,
               vector<unsigned long> &rows_read);

    /**
     * Parses a graph.
     * @param block list of parameter and their values for that system component.
     * @param name_to_id map containing names of either water sources or utilities and their corresponding IDs.
     * @param tag
     * @param l line number
     * @return parsed graph
     */
    static Graph parseGraph(vector <vector<string>> &block,
                            map<string, int> &name_to_id, const string &tag,
                            int line_no);

    /**
     * Looks for names of water sources or utilities in the arguments of a parameter and replaces them by the corresponding IDs.
     * @param block list of parameter and their values for that system component.
     * @param tag_name
     * @param line_no
     * @param param_name name of parameter with names to be replaced by IDs.
     * @param param_pos position of the value of the parameter to be looked into (some parameter requires more than one argument).
     * @param name_to_id name_to_id map containing names of either water sources or utilities and their corresponding IDs.
     */
    static void
    replaceNameById(vector <vector<string>> &block, const string &tag_name,
                    int line_no, string param_name, int param_pos,
                    map<string, int> name_to_id);

    /**
     * Finds item in map with highest value of second element in pair.
     * @tparam KeyType
     * @tparam ValueType
     * @param x relevant map
     * @return pair with maximum value of second element.
     */
    template<typename KeyType, typename ValueType>
    static pair<KeyType, ValueType> getMax(const map<KeyType, ValueType> &x) {
        using pairtype=pair<KeyType, ValueType>;
        return *max_element(x.begin(), x.end(),
                                 [](const pairtype &p1, const pairtype &p2) {
                                     return p1.second < p2.second;
                                 });
    };
};


#endif //TRIANGLEMODEL_AUXPARSERFUNCTIONS_H
