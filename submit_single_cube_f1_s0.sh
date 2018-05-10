#!/bin/bash
#PBS -N test_triangle
#PBS -l nodes=1:ppn=1
#PBS -l walltime=12:00:00
#PBS -o ./error/test_tri10.out
#PBS -e ./error/test_tri10.err
#PBS -m bea
# #PBS -M degorelick@unc.edu
cd $PBS_O_WORKDIR
time ./triangleSimulation -r 100 -t 2392 -f 0 -l 1 -d ./ -s TestFiles/decvars_hb.csv -C 0 -T 16 -X 0 -F 1
