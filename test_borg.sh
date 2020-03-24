#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=16
#SBATCH --exclusive
#SBATCH --job-name=F2_test
#SBATCH --output=output/F2_test.out
#SBATCH --error=output/F2_test.error
#SBATCH --time=08:00:00

time mpirun -np 16 ./triangleSimulation -T 2 -t 2344 -r 100 -d ./ -C -1 -D sinusoidal -E sinusoidal -F 1 -b true -o 100 -n 1000 -e 2100
