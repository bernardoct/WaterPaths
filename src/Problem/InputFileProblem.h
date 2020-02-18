//
// Created by Bernardo on 12/4/2019.
//

#ifndef TRIANGLEMODEL_INPUTFILEPROBLEM_H
#define TRIANGLEMODEL_INPUTFILEPROBLEM_H


#include "Base/Problem.h"
#include "../InputFileParser/MasterSystemInputFileParser.h"

#ifdef  PARALLEL
#include "../../Borg/borgms.h"
#endif

class InputFileProblem : public Problem {
    MasterSystemInputFileParser parser;
    string solutions_file;
    vector<unsigned long> solutions_to_run;
    vector<vector<double>> solutions_decvars;

public:
    explicit InputFileProblem(string &system_input_file);

    void setRofTablesAndRunParams();

    int functionEvaluation(double *vars, double *objs, double *consts) override;

    const vector<vector<double>> &getSolutionsDecvars() const;

    void runSimulation() override;

    void
    printObjsInLineInFile(ofstream &objs_file,
                          const vector<double> &objectives) const;

    ofstream createOutputFile() const;

    bool isOptimize() const;

    int getNFunctionEvals() const;

    int getRuntimeOutputInterval() const;

    unsigned long getNDecVars() const override;

    unsigned long getNObjectives() const override;

    int getSeed() const;

    string getOutputDir() const;


#ifdef PARALLEL
    void setProblemDefinition(BORG_Problem &problem) override;
#endif
};


#endif //TRIANGLEMODEL_INPUTFILEPROBLEM_H
