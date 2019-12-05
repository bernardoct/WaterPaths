//
// Created by Bernardo on 12/4/2019.
//

#include "InputFileProblem.h"

InputFileProblem::InputFileProblem(unsigned long n_weeks,
                                   string &system_input_file) : Problem(NON_INITIALIZED) {
    parser.preloadAndCheckInputFile(system_input_file);
}
