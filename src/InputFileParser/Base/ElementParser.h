//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_ELEMENTPARSER_H
#define TRIANGLEMODEL_ELEMENTPARSER_H

#include <string>
#include "../../SystemComponents/Utility/Utility.h"

using namespace std;

class ElementParser {
public:
    const string element_tag;

    bool check_tag(string tag);

};


#endif //TRIANGLEMODEL_ELEMENTPARSER_H
