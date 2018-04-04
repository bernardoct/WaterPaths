#!/bin/bash
#PBS -N test_mm
#PBS -l nodes=1:ppn=16
#PBS -l walltime=01:00:00
#PBS -o ./output/test_mm.out
#PBS -e ./error/test_mm.err
#PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
time ./triangleSimulation -r 1000 -t 2400 -d /scratch/bct52/ -s decvars_hb.csv
