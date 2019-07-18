#!/bin/bash
#SBATCH -n 1 -N 1 
#SBATCH --job-name=skx_test
#SBATCH --output=output/skx_test.out
#SBATCH --error=error/skx_test.err
#SBATCH -p skx-normal
#SBATCH --time=00:30:00
# #SBATCH --mail-user=bct52@cornell.edu
# #SBATCH --mail-type=all
export OMP_NUM_THREADS=96
cd $SLURM_SUBMIT_DIR
time ibrun tacc_affinity ./triangleSimulation\
	-T ${OMP_NUM_THREADS}\
       	-t 2344\
       	-r 1000\
       	-d /work/03253/tg825524/stampede2/\
       	-C 1\
	-m 0\
	-s decvars.csv\
       	-O rof_tables_test_problem/\
       	-e 0\
       	-U TestFiles/rdm_utilities_test_problem_opt.csv\
       	-W TestFiles/rdm_water_sources_test_problem_opt.csv\
       	-P TestFiles/rdm_dmp_test_problem_opt.csv\
	-p false

time ibrun tacc_affinity ./triangleSimulation\
	-T ${OMP_NUM_THREADS}\
       	-t 2344\
       	-r 1000\
       	-d /work/03253/tg825524/stampede2/\
       	-C -1\
	-f 0\
	-l 22\
	-s decvars.csv\
       	-O rof_tables_test_problem/\
       	-e 0\
       	-U TestFiles/rdm_utilities_test_problem_opt.csv\
       	-W TestFiles/rdm_water_sources_test_problem_opt.csv\
       	-P TestFiles/rdm_dmp_test_problem_opt.csv
