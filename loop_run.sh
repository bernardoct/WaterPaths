for JOB in {0..16}
do
	PBS="#!/bin/bash\n\
#PBS -N single_run_wcu\n\
#PBS -l nodes=1:ppn=16\n\
#PBS -l walltime=2:00:00\n\
#PBS -o ./output/single_run.out\n\
#PBS -e ./error/single_run.err\n\
# #PBS -m bea\n\
# #PBS -M bct52@cornell.edu\n\
cd \$PBS_O_WORKDIR\n\
module load openmpi-1.10.7-gnu-x86_64\n\
export OMP_NUM_THREADS=16\n\
N_REALIZATIONS=1000\n\
SOLUTION=270\n\
DATA_DIR=\"/scratch/bct52/\"\n\
./triangleSimulation -T \${OMP_NUM_THREADS} -t 2344 -r \${N_REALIZATIONS} -d \${DATA_DIR} -C -1 -O \${DATA_DIR}rof_tables_cac/ -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -s solutions_not_crashed.csv_repeated -e 1 -f 0 -l \$((14+$JOB)) -p false"
	echo -e $PBS | qsub
	sleep 0.5
done
