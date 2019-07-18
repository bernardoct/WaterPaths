JOB=2
DATA_DIR=/work/03253/tg825524/stampede2/
N_REALIZATIONS=1000
SLURM="#!/bin/bash\n\
#SBATCH -n $((4*$JOB)) -N ${JOB}\n\
#SBATCH --job-name=skx_test\n\
#SBATCH --output=output/skx_${JOB}.out\n\
#SBATCH --error=error/skx_${JOB}.err\n\
#SBATCH -p skx-normal\n\
#SBATCH --time=20:00\n\
#SBATCH --mail-user=bct52@cornell.edu\n\
#SBATCH --mail-type=all\n\
export OMP_NUM_THREADS=24\n\
cd \$SLURM_SUBMIT_DIR\n\
time ibrun tacc_affinity ./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_test_problem/ -e ${JOB} -U TestFiles/rdm_utilities_test_problem_opt.csv -W TestFiles/rdm_water_sources_test_problem_opt.csv -P TestFiles/rdm_dmp_test_problem_opt.csv -b true -o 25 -n 100"
	
echo -e $SLURM #| sbatch


