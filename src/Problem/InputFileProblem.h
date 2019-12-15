//
// Created by Bernardo on 12/4/2019.
//

#ifndef TRIANGLEMODEL_INPUTFILEPROBLEM_H
#define TRIANGLEMODEL_INPUTFILEPROBLEM_H


#include "Base/Problem.h"
#include "../InputFileParser/MasterSystemInputFileParser.h"

class InputFileProblem : public Problem {
    MasterSystemInputFileParser parser;
    string solutions_file;
    vector<unsigned long> solutions_to_run;
    vector<vector<double>> solutions_decvars;

public:
    explicit InputFileProblem(string &system_input_file);

    int functionEvaluation(double *vars, double *objs, double *consts) override;

    const vector<vector<double>> &getSolutionsDecvars() const;

    void runSimulation() override;

    void
    printObjsInLineInFile(ofstream &objs_file,
                          const vector<double> &objectives) const;

    ofstream createOutputFile() const;
};


#endif //TRIANGLEMODEL_INPUTFILEPROBLEM_H
