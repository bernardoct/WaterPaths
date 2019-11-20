//
// Created by Bernardo on 11/19/2019.
//

#include <algorithm>
#include <sstream>
#include <iostream>
#include "ElementParser.h"

bool ElementParser::check_tag(string tag) {
    return element_tag.compare(1, element_tag.size() - 1, tag);
}

