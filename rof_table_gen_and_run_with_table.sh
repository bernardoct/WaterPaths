#!/bin/bash
#SBATCH -n 1 -N 1
#SBATCH --job-name=calibrate
#SBATCH --output=output/calibrate.out
#SBATCH --error=error/calibrate.err
#SBATCH -p skx-dev
#SBATCH --time=00:16:00
#SBATCH --mail-user=bct52@cornell.edu
# #SBATCH --mail-type=all
cd $SLURM_SUBMIT_DIR
export OMP_NUM_THREADS=96
N_REALIZATIONS=1000
SOLUTION=0
DATA_DIR="/work/03253/tg825524/stampede2/"
./triangleSimulation -T ${OMP_NUM_THREADS}\
       	-t 2344\
       	-r ${N_REALIZATIONS}\
       	-d ${DATA_DIR}\
       	-C -1\
	-O rof_tables_test_problem\
       	-s reference_final.reference\
       	-e 0\
        -m ${SOLUTION}\
	-U TestFiles/rdm_utilities_test_problem_opt.csv\
	-W TestFiles/rdm_water_sources_test_problem_opt.csv\
        -P TestFiles/rdm_dmp_test_problem_opt.csv\
        -p false	
./triangleSimulation -T ${OMP_NUM_THREADS}\
       	-t 2344\
       	-r ${N_REALIZATIONS}\
       	-d ${DATA_DIR}\
       	-C -1\
	-O rof_tables_test_problem\
       	-s reference_final.reference\
       	-e 0\
	-f 0\
	-l 209
	-U TestFiles/rdm_utilities_test_problem_opt.csv\
	-W TestFiles/rdm_water_sources_test_problem_opt.csv\
        -P TestFiles/rdm_dmp_test_problem_opt.csv\
        -p false	
