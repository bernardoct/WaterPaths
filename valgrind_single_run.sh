#!/bin/bash
#PBS -N valgrind
#PBS -l nodes=1:ppn=16
#PBS -l walltime=0:30:00
#PBS -o ./output/valgrind.out
#PBS -e ./error/valgrind.err
# #PBS -m bea
# #PBS -M bct52@cornell.edu
cd $PBS_O_WORKDIR
module load openmpi-1.10.7-gnu-x86_64
module load valgrind-3.8.1
set OMP_NUM_THREADS=16
N_REALIZATIONS=16
SOLUTION=0
#./triangleSimulation_o2 -T 16 -t 2344 -r ${N_REALIZATIONS} -d /scratch/bct52/ -s problem_dvs.csv -C 1 -O rof_tables_valgrind -m 0 -e 0
valgrind --tool=memcheck --leak-check=full --track-origins=yes --log-file=valgrind_output_long_run.out ./triangleSimulation -T 16 -t 234 -r ${N_REALIZATIONS} -d /scratch/bct52/ -s problem_dvs.csv -C -1 -O rof_tables/ -m ${SOLUTION} 
