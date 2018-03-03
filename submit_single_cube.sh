#!/bin/bash
#PBS -N test_durham
#PBS -l nodes=1:ppn=1
#PBS -l walltime=01:00:00
#PBS -o ./error/test_durham.out
#PBS -e ./error/test_durham.err
#PBS -m bea
# #PBS -M degorelick@unc.edu
cd $PBS_O_WORKDIR
time ./src/main -r 144 -t 2392 -f 0 -l 1 -d ./ -s TestFiles/sols16.txt -C 0 -T 1 -S 1
