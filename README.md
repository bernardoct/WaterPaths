# WaterPaths
WaterPaths - A utility planning and management tool based off the NC Triangle model.

## What is included in this repository
- Source code (/src).
- NC Triangle water planning and management problem (src/Problem/Triangle.cpp).
- NC Triangle data (/TestFiles).
- Sample input time series (/TestFiles/inflows, evaporation, demand) with 64 realizations each.

## Compiling WaterPaths
Use Makefile provided in source directory. E.g., if using GCC run `make gcc`.

## Running WaterPaths
Running `triangleSimulation -?` will give you a list of flags to call WaterPaths with.

### Running WaterPaths in simulation mode
For running a reduced version of the NC Triangle model in full simulation mode, run:
`./triangleSimulation -T 4 -t 2344 -r 64 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -s sample_solutions.csv -m 0`.
The argument of flag -T is the number of threads to be created using OpenMP. If -T flag is not used, the 64 realizations will be distributed across all physical and virtual cores.

There are three options for decreasing runtime for testing purposes that can be used individually or together:
- Decreasing the number of realizations (-r flag)
- Decreasing the simulation time (-t flag) to no less than 53 (weeks)
- Generating pre-computed risk-of-failure (ROF) tables to be used during runs. Keep in mind that if using a different set of inflow, evaporation, of RDM files will warrant new tables. To generate tables, run `./triangleSimulation -T 4 -t 2344 -r 64 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -s sample_solutions.csv -m 0 -C 1`. To run a function evaluation using the tables, set value of -C flag to -1.

### Runnning WaterPaths with Borg MS in optimization mode
To get a copy of Borg, go to Borg's [official website](http://borgmoea.org/) and request access to the source  code (free for non-commercial use). You should soon after get an e-mail with a link to its repository, where you can download the source code from.

To run WaterPaths in optimization mode, WaterPaths needs to be re-compiled with Borg and the appropriate flags must be passed when calling it from the command line with a program like mpirun. To do so, follow the following steps:
1. In order to run WaterPaths with Borg, the library libborgms.a must be provided in the /lib folder. To do so, borgmoea.org/ and request a licence, move Borg files to the folder /borg, compile borg by running `make mpi` in /borg, and finally move the file libborgms.a to the /lib folder.
2. After libborgms.a is compiled and place in /lib, compile WaterPaths with `make borg`.
3. Copy directories cube:/scratch/bct52/TestFiles and cube:/scratch/bct52/rof_tables_cac/ to your directory of preference, or `${DATA_DIR}` (both mentioned directories must be in the same directory). 
4. Run WaterPaths with `mpiexec --hostfile nodefile -np 3 -x OMP_NUM_THREADS ./triangleSimulation -T 5 -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O ${DATA_DIR}rof_tables_cac/ -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -b true -n 202 -o 100 -e 1`. The WaterPaths call above is optimized for a node with 16 cores and will spawn 3 MPI processes running on 5 threats each. Be sure to change the nodefile (see single_run_borgms.sh) and the value of the -T flag for nodes with different number of cores. 
5. The run should take less than 10 minutes if 16 or more cores are being used. The two output files of this run should now be in `${DATA_DIR}/output/`. The NC_output_MM_S1_N202.set file should consist on a space-separated matrix with 20-80 rows by 63 columns. The /scratch/bct52/output/NC_runtime_MM_S1_N202.runtime file should have between 90 and 200 lines.

## Using Nix or NixOS

Nix is a reproducible package management system with several other features. Start up a nix shell with
(this assumes ~/.ssh/id_rsa is linked to your BitBucket account and that you have access to Borg on
BitBucket):

```
nix-shell -I ssh-config-file=./nix/.ssh-standard-config -I ssh-auth-sock=$SSH_AUTH_SOCK nix/shell.nix
```

You should then be able to build WaterPaths as described above.

## Using Containers

The containers encompass the nix instructions described above, but use
a container with nix, so there is no need to install it yourself.

### Docker

#### Building

As WaterPaths depends on Borg, you will need to:

1. [obtain a license](http://borgmoea.org/) for Borg, after which you should obtain access to the repository on BitBucket
2. Generate an ssh key, e.g. `ssh-keygen -t rsa -b 4096 -N "" -C "your_email@example.com" -f ~/.ssh/bitbucket_no_pass`. Your email address should ideally be an email address associated with your bitbucket account.
3. Add the public component of the ssh key to your BitBucket account (`bitbucket_no_pass.pub` in the example above).
4. Copy the **private** ssh key to `id_rsa` in this repository's root (note that `id_rsa` is `.gitignore`d, but still do be careful): `cp ~/.ssh/bitbucket_no_pass ./id_rsa`, assuming you are in the WaterPaths directory.

Then, we can proceed as we normally might:

```
source build-docker.sh
```

#### Running Locally

```
source docker-compose.sh up --scale mpi_head=1 --scale mpi_node=3
docker exec -u nixuser -it waterpaths_mpi_head_1 /bin/sh
cd ~/WaterPaths
```

Then you can run with Borg in MPI-mode, e.g.:

```
mpirun -n 4 triangleSimulation -T 4 -t 2344 -r 16 -d /home/nixuser/WaterPaths/ -s sample_solutions.csv -m 0 -b t
rue
```

Note that we use `-n 4`: `-n 5` would not work since we only have 4 nodes (really, containers): 3 nodes plus 1 head node.

Now you should be able to follow the "Running WaterPaths" directions above.

### Singularity

(Work in progress)
