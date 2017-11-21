#!/bin/bash

# How many nodes, processors, and hours to request for each seed?
SOLUTIONS=$(seq 0 15)

for SOLUTION in ${SOLUTIONS}
do
	NAME=SOLUTION_${SOLUTION}
	echo "Submitting solution: ${SOLUTION}"

PBS="#!/bin/bash\n\
#PBS -N ${NAME}\n\
#PBS -l nodes=1:ppn=16\n\
#PBS -l walltime=1:30:00\n\
#PBS -o ./output/test_bs_${SOLUTION}.out\n\
#PBS -e ./error/test_bs_${SOLUTION}.err\n\
cd \$PBS_O_WORKDIR\n\
time ./triangleSimulation -r 1000 -t 2400 -d /scratch/bct52/ -s decvars_hb.csv -m ${SOLUTION}"

	echo -e ${PBS} | qsub 
	sleep 0.5
	echo "done."
done
