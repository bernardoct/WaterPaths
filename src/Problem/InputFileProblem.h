//
// Created by Bernardo on 12/4/2019.
//

#ifndef TRIANGLEMODEL_INPUTFILEPROBLEM_H
#define TRIANGLEMODEL_INPUTFILEPROBLEM_H


#include "Base/Problem.h"
#include "../InputFileParser/MasterSystemInputFileParser.h"

class InputFileProblem : Problem {
    MasterSystemInputFileParser parser;
public:
    InputFileProblem(unsigned long n_weeks,
                     string &system_input_file);
};


#endif //TRIANGLEMODEL_INPUTFILEPROBLEM_H
