#!/bin/bash
#PBS -N borgms_run
#PBS -l nodes=2:ppn=16
#PBS -l walltime=2:00:00
#PBS -o ./output/borgms_run_hybrid.out
#PBS -e ./error/borgms_run_hybrid.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
module load openmpi-1.10.7-gnu-x86_64
export OMP_NUM_THREADS=4
N_REALIZATIONS=64
DATA_DIR=${PBS_O_WORKDIR}/ #"/scratch/bct52/"

set -x
cd "$PBS_O_WORKDIR"

# Construct a copy of the hostfile with only 16 entries per node.
# MPI can use this to run 16 tasks on each node.
export TASKS_PER_NODE=4
uniq "$PBS_NODEFILE"|awk -v TASKS_PER_NODE="$TASKS_PER_NODE" '{for(i=0;i<TASKS_PER_NODE;i+=1) print}' > nodefile

cat nodefile
mpiexec --hostfile nodefile -n 8 -x OMP_NUM_THREADS ./triangleSimulation -T ${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_cac/ -b true -n 500 -o 100 -e 0
