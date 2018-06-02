#!/bin/sh
FORMULATION=3
for SCENARIO in 0 1 2 3 4 5 6
do
        NAME=F_${FORMULATION}_S_${SCENARIO}
        echo "Submitting formulation ${FORMULATION} scenario ${SCENARIO}"
        PBS="\
        #PBS -N ${NAME}\n\
        #PBS -l nodes=1:ppn=2\n\
        #PBS -l walltime=01:00:00\n\
        #PBS -o ./error/test_bs_${FORMULATION}_${SCENARIO}.out\n\
        #PBS -e ./error/test_bs_${FORMULATION}_${SCENARIO}.err\n\
        cd \$PBS_O_WORKDIR\n\
        time ./triangleSimulation -r 10 -t 2392 -d ./ -s TestFiles/decvars_hb.csv -T 2 -C 0 -f 0 -l 1 -F ${FORMULATION} -X ${SCENARIO}"
        echo -e ${PBS} | qsub
        sleep 0.5
        echo "done."
done