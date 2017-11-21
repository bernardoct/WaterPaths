#!/bin/bash
#SBATCH -n 136 -N 2 #SBATCH --job-name=single_run
#SBATCH --output=output/single_run.out
#SBATCH --error=error/single_run.err
#SBATCH -p normal
#SBATCH --time=01:15:00
export OMP_NUM_THREADS=68
cd $SLURM_SUBMIT_DIR
./triangleSimulation -r 12 -t 2316 -d /work/03253/tg825524/stampede2 -s /decvars.csv
