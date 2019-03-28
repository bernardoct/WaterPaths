#!/bin/bash
#PBS -N borgms_single_run_315
#PBS -l nodes=8:ppn=16
#PBS -l walltime=4:00:00
#PBS -o ./output/borgms_single_run_315.out
#PBS -e ./error/borgms_single_run_315.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
set OMP_NUM_THREADS=16
N_REALIZATIONS=1000
DATA_DIR="/scratch/bct52/"
mpiexec -n 128 ./triangleSimulation -T 1 -t 2344 -r 16 -d ${DATA_DIR} -C -1 -O rof_tables -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -b true -n 10000 -o 1000
