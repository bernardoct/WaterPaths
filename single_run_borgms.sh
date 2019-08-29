#!/bin/bash
#PBS -N borgms_run
#PBS -l nodes=8:ppn=16
#PBS -l walltime=48:00:00
#PBS -o ./output/borgms_run_8.out
#PBS -e ./error/borgms_run_8.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
export OMP_NUM_THREADS=5
N_REALIZATIONS=1000
DATA_DIR="/scratch/bct52/"

# Construct a copy of the hostfile with only 8 entries per node.
# MPI can use this to run 8 tasks on each node.
export TASKS_PER_NODE=3
uniq "$PBS_NODEFILE"|awk -v TASKS_PER_NODE="$TASKS_PER_NODE" '{for(i=0;i<TASKS_PER_NODE;i+=1) print}' > nodefile

cat nodefile

time mpiexec --hostfile nodefile -np 24 -x OMP_NUM_THREADS ./triangleSimulation -T ${OMP_NUM_THREADS}\
       	-t 2344\
       	-r ${N_REALIZATIONS}\
       	-d ${DATA_DIR}\
       	-C -1\
       	-O rof_tables_test_problem/\
       	-e 8\
	-U TestFiles/rdm_utilities_test_problem_opt.csv\
	-W TestFiles/rdm_water_sources_test_problem_opt.csv\
	-P TestFiles/rdm_dmp_test_problem_opt.csv\
	-b true\
	-o 2500\
	-n 20000

