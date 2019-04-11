#!/bin/bash
#PBS -N single_run_wcu
#PBS -l nodes=1:ppn=16
#PBS -l walltime=0:10:00
#PBS -o ./output/single_run.out
#PBS -e ./error/single_run.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
set OMP_NUM_THREADS=16
N_REALIZATIONS=1000
SOLUTION=270
./triangleSimulation -T ${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d /scratch/bct52/ -s solutions_not_crashed.csv -C -1 -O rof_tables/ -m ${SOLUTION} 
