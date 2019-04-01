#!/bin/bash
#PBS -N borgms_run
#PBS -l nodes=2:ppn=16
#PBS -l walltime=1:00:00
#PBS -o ./output/borgms_run_hybrid.out
#PBS -e ./error/borgms_run_hybrid.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
module load openmpi-1.10.7-gnu-x86_64
set OMP_NUM_THREADS=16
N_REALIZATIONS=10
DATA_DIR="/scratch/bct52/"

set -x
cd "$PBS_O_WORKDIR"

# Construct a copy of the hostfile with only 16 entries per node.
# MPI can use this to run 16 tasks on each node.
export TASKS_PER_NODE=1
uniq "$PBS_NODEFILE"|awk -v TASKS_PER_NODE="$TASKS_PER_NODE" '{for(i=0;i<TASKS_PER_NODE;i+=1) print}' > nodefile

cat nodefile
mpiexec --hostfile nodefile -n 2 -x OMP_NUM_THREADS valgrind --tool=memcheck --leak-check=full --track-origins=yes --log-file=valgrind_output_long_run.out ./triangleSimulation -T ${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables/ -b true -n 30000 -o 1000 -e 0
