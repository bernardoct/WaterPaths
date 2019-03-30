#!/bin/bash
#PBS -l nodes=1:ppn=16
#PBS -l walltime=24:00:00
#PBS -e serial_borg.err
#PBS -o serial_borg.out

cd $PBS_O_WORKDIR

./triangleSimulation -T 16 -t 2344 -r 1000 -d /scratch/dfg42/WaterPathsOut/ -C -1 -o 1000 -b true -n 1000