#!/bin/bash
SCEN=0
FORM=0
#PBS -N test_triangle
#PBS -l nodes=1:ppn=1
#PBS -l walltime=12:00:00
#PBS -o ./error/test_tri00.out
#PBS -e ./error/test_tri00.err
#PBS -m bea
# #PBS -M degorelick@unc.edu
cd $PBS_O_WORKDIR
time ./triangleSimulation -r 1 -t 260 -f 0 -l 1 -d ./ -s TestFiles/decvars_hb.csv -C 0 -T 1 -X ${SCEN} -F ${FORM}
