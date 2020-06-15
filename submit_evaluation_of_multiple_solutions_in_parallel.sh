#!/bin/bash
SOLUTIONS=(0 1 10 20 30 50 94)
NPROCS=(17 34 51 68 136 204 272)

for I in {0..6}
do
	echo "submitting solution: ${SOLUTIONS[$I]} with ${NPROCS[$I]}"

PBS="#!/bin/bash\n\
#SBATCH -n ${NPROCS[$I]} -N 1 #SBATCH --job-name=single_run_sol_${SOLUTIONS[$I]}_${NPROCS[$I]}\n\
#SBATCH --output=output/single_run_${SOLUTIONS[$I]}_${NPROCS[$I]}.out\n\
#SBATCH --error=error/single_run_${SOLUTIONS[$I]}_${NPROCS[$I]}.err\n\
#SBATCH -p normal\n\
#SBATCH --time=00:30:00\n\
export OMP_NUM_THREADS=${NPROCS[$I]}\n\
cd \$SLURM_SUBMIT_DIR\n\
./triangleSimulation -T ${NPROCS[$I]} -r 1000 -t 2344 -d ${WORK}/ -s /decvars_hb.csv -m ${SOLUTIONS[$I]} -C -1 -U TestFiles/rdm_utilities.csv -P TestFiles/rdm_dmp.csv -W TestFiles/rdm_water_sources.csv -y TestFiles/bootstrap_samples/all_1000_realizations.csv"

	echo -e ${PBS} | sbatch
	sleep 0.5
	echo "done."
done
