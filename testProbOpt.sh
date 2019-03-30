#!/bin/bash
#PBS -l nodes=20:ppn=16
#PBS -l walltime=10:00:00
#PBS -e waterpaths_borg.err
#PBS -o waterpaths_borg.out

cd $PBS_O_WORKDIR

mpiexec -n 320 ./triangleSimulation -T 4 -t 2344 -r 1000 -d /scratch/dfg42/WaterPathsOut/ -C -1 -o 1000 -b true -n 1000 -e 0 -i 1


