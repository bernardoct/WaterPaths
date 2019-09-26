DATA_DIR=/work/03253/tg825524/stampede2/
N_REALIZATIONS=1000
JOB=4
for JOB in 8 #16 32 #40 48 56 64 96 128
do
SLURM="#!/bin/bash\n\
#SBATCH -n $((4*$JOB)) -N ${JOB}\n\
#SBATCH --time=$((265/$JOB)):00:00\n\
#SBATCH --job-name=skx_${JOB}\n\
#SBATCH --output=output/skx_${JOB}.out\n\
#SBATCH --error=error/skx_${JOB}.err\n\
#SBATCH -p skx-normal\n\
#SBATCH --mail-user=bct52@cornell.edu\n\
#SBATCH --mail-type=all\n\
export OMP_NUM_THREADS=24\n\
cd \$SLURM_SUBMIT_DIR\n\
time ibrun tacc_affinity ./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_test_problem/ -e ${JOB} -U TestFiles/rdm_utilities_test_problem_opt.csv -W TestFiles/rdm_water_sources_test_problem_opt.csv -P TestFiles/rdm_dmp_test_problem_opt.csv -b true -o 2500 -n 125000"

echo -e $SLURM | sbatch
sleep 0.5
done
