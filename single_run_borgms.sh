#!/bin/bash
#PBS -N borgms_run
#PBS -l nodes=8:ppn=16
#PBS -l walltime=2:00:00
#PBS -o ./output/borgms_run.out
#PBS -e ./error/borgms_run.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
set OMP_NUM_THREADS=16
N_REALIZATIONS=16
DATA_DIR="/scratch/bct52/"
#./triangleSimulation -T ${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C 1 -O rof_tables -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -e 0
mpiexec -n 128 ./triangleSimulation -T 1 -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C 0 -O rof_tables -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -b true -n 200 -o 100 -e 0
