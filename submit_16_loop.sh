#!/bin/bash

# How many nodes, processors, and hours to request for each seed?
SCENARIOS=$(seq 1 4)

for SCENARIO in ${SCENARIOS}
do
	NAME=SCENARIO_${SCENARIO}
	echo "Submitting solution: ${SCENARIO}"

PBS="#!/bin/bash\n\
#PBS -N ${NAME}\n\
#PBS -l nodes=1:ppn=16\n\
#PBS -l walltime=04:00:00\n\
#PBS -o ./error/test_bs_${SCENARIO}.out\n\
#PBS -e ./error/test_bs_${SCENARIO}.err\n\
cd \$PBS_O_WORKDIR\n\
time ./DurhamModel -r 100 -t 2392 -d ./ -s TestFiles/decvars_hb.csv -S ${SCENARIO} -T 16 -C 0 -f 0 -l 20"

	echo -e ${PBS} | qsub 
	sleep 0.5
	echo "done."
done
