DATA_DIR=/work/03253/tg825524/stampede2/
N_REALIZATIONS=1000
for JOB in 8 16 32 64 128
do
SLURM="#!/bin/bash\n\
#SBATCH -n $((4*$JOB)) -N ${JOB}\n\
#SBATCH --job-name=skx_test_$JOB\n\
#SBATCH --output=output/skx_MS_${JOB}.out\n\
#SBATCH --error=error/skx_MS_${JOB}.err\n\
#SBATCH -p skx-normal\n\
#SBATCH --time=$((225/$JOB+1)):00:00\n\
#SBATCH --mail-user=bct52@cornell.edu\n\
#SBATCH --mail-type=all\n\
export OMP_NUM_THREADS=24\n\
cd \$SLURM_SUBMIT_DIR\n\
time ibrun tacc_affinity ./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_test_problem/ -e ${JOB} -U TestFiles/rdm_utilities_test_problem_opt.csv -W TestFiles/rdm_water_sources_test_problem_opt.csv -P TestFiles/rdm_dmp_test_problem_opt.csv -b true -o 2500 -n 125000"
	
echo -e $SLURM | sbatch
sleep 0.5
done

