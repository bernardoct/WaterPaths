#!/bin/bash
#PBS -N single_run_wcu
#PBS -l nodes=1:ppn=16
#PBS -l walltime=2:00:00
#PBS -o ./output/single_run.out
#PBS -e ./error/single_run.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
export OMP_NUM_THREADS=16
N_REALIZATIONS=1000
SOLUTION=270
DATA_DIR="/scratch/bct52/"
./triangleSimulation -T ${OMP_NUM_THREADS}\
       	-t 2344\
       	-r ${N_REALIZATIONS}\
       	-d ${DATA_DIR}\
       	-C 1\
       	-O ${DATA_DIR}rof_tables_test_problem/\
       	-s test_DVs_table_shifts.csv\
       	-e 0\
        -m 0
