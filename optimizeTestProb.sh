#!/bin/bash
#PBS -N optimizeTestProb
#PBS -l nodes=16:ppn=16
#PBS -l walltime=10:00:00
#PBS -o optimizeTest.out
#PBS -e optimizeTest.err

cd $PBS_O_WORKDIR
mpirun -n 256 ./triangleSimulation -T 2 -t 2344 -r 1000 -d /scratch/dfg42/WaterPathsOut/ -C -1 -o 1000 -b true -n 10000 -e 0 -i 1
