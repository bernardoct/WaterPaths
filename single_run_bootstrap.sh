#!/bin/bash
#PBS -N single_run_wcu
#PBS -l nodes=1:ppn=16
#PBS -l walltime=0:10:00
#PBS -o ./output/single_run_bootstrap.out
#PBS -e ./error/single_run_bootstrap.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
set OMP_NUM_THREADS=16
N_REALIZATIONS=1000
./triangleSimulation -T 16 -t 2344 -r ${N_REALIZATIONS} -d /scratch/bct52/ -s solutions_not_crashed.csv -C -1 -O rof_tables_valgrind/ -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -m 270 -S 300 -A 5000
