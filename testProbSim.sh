#PBS -l nodes=1:ppn=16
#PBS -l walltime=10:00:00
#PBS -j oe
#PBS -o test_sim.out

cd $PBS_O_WORKDIR
set OMP_NUM_THREADS=16

./triangleSimulation -T 16 -t 2344 -r 1000 -d /scratch/dfg42/WaterPathsOut/ -C -1 -s /test_DVs_all_off.csv
