#!/bin/bash
#SBATCH -n 1 -N 1 
#SBATCH --job-name=test_wp_input_file
#SBATCH --output=output/test_wp_input_file.out
#SBATCH --error=error/test_wp_input_file.err
#SBATCH --time=12:30:00
# #SBATCH --mail-user=bct52@cornell.edu
# #SBATCH --mail-type=all
#SBATCH --exclusive
export OMP_NUM_THREADS=16
cd $SLURM_SUBMIT_DIR
#module load valgrind/3.15.0
#valgrind --tool=callgrind --instr-atstart=no ./waterpaths -I Tests/test_input_file.wp
#./waterpaths -I Tests/test_input_file.wp
./waterpaths -I Tests/test_input_file_dv_file.wp


