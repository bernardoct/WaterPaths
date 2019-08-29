DATA_DIR="/scratch/03253/tg825524/"
N_REALIZATIONS=1000
SOLS_FILE_NAME="reference_final_filtered.reference"
RDM_PER_JOB=200
N_NODES=50
for RDM in $(seq 0 $RDM_PER_JOB 4999)
do
	SLURM="#!/bin/bash\n\
#SBATCH -n $N_NODES -N $N_NODES\n\
#SBATCH --time=03:20:00\n\
# #SBATCH --time=02:00:00\n\
#SBATCH --job-name=skx_reeval_${RDM}_to_$(($RDM+$RDM_PER_JOB-1))\n\
#SBATCH --output=output/skx_reeval_${RDM}_to_$(($RDM+$RDM_PER_JOB-1)).out\n\
#SBATCH --error=error/skx_reeval_${RDM}_to_$(($RDM+$RDM_PER_JOB-1)).err\n\
#SBATCH -p skx-normal\n\
# #SBATCH -p skx-dev\n\
#SBATCH --mail-user=bct52@cornell.edu\n\
#SBATCH --mail-type=all\n\
export OMP_NUM_THREADS=96\n\
cd \$SLURM_SUBMIT_DIR\n\
module load python\n
ibrun tacc_affinity python reeval.py \$OMP_NUM_THREADS $N_REALIZATIONS $DATA_DIR $RDM $SOLS_FILE_NAME $N_NODES $RDM_PER_JOB"
	echo -e $SLURM | sbatch
	sleep 0.5
done
#./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C 1 -O rof_tables_test_problem_RDM_${RDM}/ -e 0 -U TestFiles/rdm_utilities_test_problem_reeval.csv -W TestFiles/rdm_water_sources_test_problem_reeval.csv -P TestFiles/rdm_dmp_test_problem_reeval.csv -R ${RDM} -s reference_set_test_problem.reference -m 0\n\

#time ./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_test_problem_${RDM}/ -e 0 -U TestFiles/rdm_utilities_test_problem_reeval.csv -W TestFiles/rdm_water_sources_test_problem_reeval.csv -P TestFiles/rdm_dmp_test_problem_reeval.csv -R ${RDM} -s reference_set_test_problem.reference -f 0 -l $N_SOLS"
#time seq 0 3 | xargs -n1 -P4 -I@ ./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O rof_tables_test_problem_${RDM}/ -e 0 -U TestFiles/rdm_utilities_test_problem_reeval.csv -W TestFiles/rdm_water_sources_test_problem_reeval.csv -P TestFiles/rdm_dmp_test_problem_reeval.csv -R ${RDM} -s reference_set_test_problem.reference -f \$((\$@*$N_SOLS/4)) -l \$(((\$@+1)*$N_SOLS/4))"
