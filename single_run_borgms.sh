#!/bin/bash
#PBS -N borgms_run
#PBS -l nodes=1:ppn=16
#PBS -l walltime=1:00:00
#PBS -o ./output/borgms_run.out
#PBS -e ./error/borgms_run.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
export OMP_NUM_THREADS=5
N_REALIZATIONS=24
DATA_DIR="/scratch/bct52/"

# Construct a copy of the hostfile with only 16 entries per node.
# MPI can use this to run 16 tasks on each node.
export TASKS_PER_NODE=3
uniq "$PBS_NODEFILE"|awk -v TASKS_PER_NODE="$TASKS_PER_NODE" '{for(i=0;i<TASKS_PER_NODE;i+=1) print}' > nodefile

cat nodefile

time mpiexec --hostfile nodefile -np 3 -x OMP_NUM_THREADS ./triangleSimulation -T 5 -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_cac/ -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -b true -n 202 -o 100 -e 1
