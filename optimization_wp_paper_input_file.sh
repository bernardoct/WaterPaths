#!/bin/bash
#SBATCH -n 48 -N 16
#SBATCH --time=140:00:00
#SBATCH --job-name=borg_input_file
#SBATCH --output=output/borg_input_file.out
#SBATCH --error=error/borg_input_file.err
#SBATCH --mail-user=bct52@cornell.edu
#SBATCH --mail-type=all
#SBATCH --exclusive
export OMP_NUM_THREADS=5
cd $SLURM_SUBMIT_DIR
module load valgrind/3.15.0
#module load remora/1.8.3  # various types of profiling. Check on Remora's documentation.

# # Profiling for optimization with WaterPaths skipping input reading.
# calgrind_control -i on  
# mpirun -np 3 valgrind --tool=callgrind --instr-atstart=no ./waterpaths -I Tests/test_input_file_borg.wp

# # Profiling for optimization with WaterPaths including input reading.
# mpirun -np 3 valgrind --tool=callgrind ./waterpaths -I Tests/test_input_file_borg.wp

# # Memory check for optimization with WaterPaths (there should be small memory leaks related to OpenMP and to the graph code.
# mpirun -np 3 valgrind --tool=memcheck --leak-check=full --track-origins=yes --xml=yes --xml-file=valgrind.xml ./waterpaths -I Tests/test_input_file_borg.wp

# Production run optimization with WaterPaths.
mpirun -np 48 ./waterpaths -I Tests/test_input_file_borg.wp
