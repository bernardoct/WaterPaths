//
// Created by bernardo on 2/25/17.
//

#ifndef TRIANGLEMODEL_LP_SOLVER_H
#define TRIANGLEMODEL_LP_SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../Utils/Constants.h"

using namespace Constants;

class LpSimplexSolver {

/*
  What: Simplex in C
  AUTHOR: GPL(C) moshahmed/at/gmail.

  What: Solves LP Problem with Simplex:
    { maximize cx : Ax <= b, x >= 0 }.
  Input: { m, n, Mat[m x n] }, where:
    b = mat[1..m,0] .. column 0 is b >= 0, so x=0 is a basic feasible solution.
    c = mat[0,1..n] .. row 0 is z to maximize, note c is negated in input.
    A = mat[1..m,1..n] .. constraints.
    x = [x1..xm] are the named variables in the problem.
    Slack variables are in columns [m+1..m+n]

  USAGE:
    1. Problem can be specified before main function in source code:
      c:\> vim mosplex.c
      Tableau tab  = { m, n, {   // tableau size, row x columns.
          {  0 , -c1, -c2,  },  // Max: z = c1 x1 + c2 x2,
          { b1 , a11, a12,  },  //  b1 >= a11 x1 + a12 x2
          { b2 , a21, a22,  },  //  b2 >= a21 x1 + a22 x2
        }
      };
      c:\> cl /W4 mosplex.c  ... compile this file.
      c:\> mosplex.exe problem.txt > solution.txt

    2. OR Read the problem data from a file:
      $ cat problem.txt
            m n
            0  -c1 -c2
            b1 a11 a12
            b2 a21 a11
      $ gcc -Wall -g mosplex.c  -o mosplex
      $ mosplex problem.txt > solution.txt
*/

private:
    static constexpr double epsilon = 1.0e-8;

//    void read_tableau(Tableau *tab, const char *filename);
//
//    void pivot_on(Tableau *tab, int row, int col);
//
//    int find_pivot_column(Tableau *tab);
//
//    int find_pivot_row(Tableau *tab, int pivot_col);
//
//    void add_slack_variables(Tableau *tab);
//
//    void check_b_positive(Tableau *tab);
//
//    int find_basis_variable(Tableau *tab, int col);
//
//    void print_optimal_vector(Tableau *tab, char *message);
//
//    void nl(int k);

public:

    static void simplex(Tableau *tab);

    static void print_tableau(Tableau *tab, const char *mes);
};


#endif //TRIANGLEMODEL_LP_SOLVER_H
