from mpi4py import MPI
import numpy as np
import subprocess, sys, time
import os

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

OMP_NUM_THREADS = sys.argv[1]
N_REALIZATIONS = sys.argv[2]
DATA_DIR = sys.argv[3]
RDM = int(sys.argv[4])
SOLS_FILE_NAME = sys.argv[5]
N_NODES = int(sys.argv[6])
N_RDM_PER_JOB = int(sys.argv[7])

print(sys.argv[1:8])

N_SOLS = np.loadtxt(DATA_DIR + SOLS_FILE_NAME, delimiter=',').shape[0]

for i in range(N_RDM_PER_JOB / N_NODES):
	command_gen_tables1 = "./triangleSimulation -T {} -t 2344 -r {} -d {} -C 1 -O rof_tables_test_problem_rdm_{} -e 0 -U TestFiles/rdm_utilities_test_problem_reeval.csv -W TestFiles/rdm_water_sources_test_problem_reeval.csv -P TestFiles/rdm_dmp_test_problem_reeval.csv -R {} -s {} -m 0 -p false".format(OMP_NUM_THREADS, N_REALIZATIONS, DATA_DIR, RDM + rank + N_NODES * i, RDM + rank + N_NODES * i, SOLS_FILE_NAME)
	
	command_run_rdm1 = "./triangleSimulation -T {} -t 2344 -r {} -d {} -C -1 -O rof_tables_test_problem_rdm_{} -e 0 -U TestFiles/rdm_utilities_test_problem_reeval.csv -W TestFiles/rdm_water_sources_test_problem_reeval.csv -P TestFiles/rdm_dmp_test_problem_reeval.csv -R {} -s {} -f 0 -l {} -p false".format(OMP_NUM_THREADS, N_REALIZATIONS, DATA_DIR, RDM + rank + N_NODES * i, RDM + rank + N_NODES * i, SOLS_FILE_NAME, N_SOLS)
	
	print(command_gen_tables1)
	print(command_run_rdm1)
	os.system(command_gen_tables1)
	os.system(command_run_rdm1)

comm.Barrier()
