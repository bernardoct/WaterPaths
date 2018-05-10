#!/bin/bash

# How many nodes, processors, and hours to request for each seed?
for FORMULATION in 0 1 2
do
    for SCENARIO in 0 1 2 3 4
    do
        NAME=F_${FORMULATION}_S_${SCENARIO}
        echo "Submitting formulation ${FORMULATION} scenario ${SCENARIO}"

    PBS="#!/bin/bash\n\
    #PBS -N ${NAME}\n\
    #PBS -l nodes=1:ppn=1\n\
    #PBS -l walltime=08:00:00\n\
    #PBS -o ./error/test_bs_${FORMULATION}_${SCENARIO}.out\n\
    #PBS -e ./error/test_bs_${FORMULATION}_${SCENARIO}.err\n\
    cd \$PBS_O_WORKDIR\n\
    time ./triangleSimulation -r 100 -t 2392 -d ./ -s TestFiles/decvars_hb.csv -X ${SCENARIO} -T 16 -C 0 -f 0 -l 1 -F ${FORMULATION}"

        echo -e ${PBS} | qsub
        echo "done."
    done
done