#!/bin/bash
#PBS -N test_durham
#PBS -l nodes=1:ppn=16
#PBS -l walltime=72:00:00
#PBS -o ./error/test_durham1.out
#PBS -e ./error/test_durham1.err
#PBS -m bea
# #PBS -M degorelick@unc.edu
cd $PBS_O_WORKDIR
time ./DurhamModel -r 1000 -t 2392 -f 0 -l 100 -d ./ -s TestFiles/decvars_hb.csv -C 0 -T 16 -S 1
