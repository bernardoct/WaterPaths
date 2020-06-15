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
