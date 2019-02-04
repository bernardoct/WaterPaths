# WaterPaths
WaterPaths - A utility planning and management tool based off the NC Triangle model.

## What is included in this repository
- Source code (/src)
- NC Triangle water planning and management problem (src/Problem/Triangle.cpp)
- NC Triangle data (/TestFiles)
- Shortened input time series (/TestFiles/*few_records)

## Compiling WaterPaths
Use Makefile provided in source directory. E.g., if using GCC run `make gcc`

## Running WaterPaths
Running `triangleSimulation -?` will give you a list of flags to call WaterPaths with.

### Running WaterPaths in simulation mode
For running a reduced version of the NC Triangle model in full simulation mode, run:
`./triangleSimulation -T 4 -t 2344 -r 16 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -I _few_records -s decvars.csv -m 0`.
The argument of flag -T is the number of threads to be created using OpenMP. If no value is provided for the -T flag all cores of the computer will be used.

There are three options for decreasing runtime for testing purposes that can be used individually or together:
- Decreasing the number of realizations (-r flag)
- Decreasing the simulation time (-t flag) to no less than 53 (weeks)
- Generating pre-computed risk-of-failure tables to be used during runs. Keep in mind that if using a different set of inflow, evaporation, of RDM files will warrant new tables. To generate tables, run `./triangleSimulation -T 4 -t 2344 -r 88 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -I _few_records -s decvars.csv -m 0 -C 1`. To use the tables, set value of -C flag to -1.

### Runnning WaterPaths with Borg in optimization mode
To get a copy of Borg, go to Borg's [official website](http://borgmoea.org/) and request access to the source  code (free for non-commercial use). You should soon after get an e-mail with a link to its repository, where you can download the source code from.

To run WaterPaths in optimization mode, WaterPaths needs to be re-compiled with Borg and the appropriate flags must be passed when calling it from the command line with a program like mpirun. To do so, follow the following steps:
1. In order to run WaterPaths with Borg, the library libborgmm.a must be provided in the /lib folder. To do so, borgmoea.org/ and request a licence, move Borg files to the folder /borg, compile borg by running `make mpi` in /borg, and finally move the file libborgmm.a to the /lib folder.
2. After libborgmm.a is compiled and place in /lib, compile WaterPaths with `make borg`.
3. Run WaterPaths with `mpirun -n 5 triangleSimulation -T 4 -t 2344 -r 16 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -I _few_records -s decvars.csv -m 0 -b true`. Keep in mind that the number of MPI processes must be at least 3 but the minimum recommended is 5. also keep in mind that each MPI process will try to create the number of threads specified with flag -T.

## Using Nix or NixOS

Nix is a reproducible package management system with several other features. Start up a nix shell with
(this assumes ~/.ssh/id_rsa is linked to your BitBucket account and that you have access to Borg on
BitBucket):

```
nix-shell -I ssh-config-file=./nix/.ssh-standard-config -I ssh-auth-sock=$SSH_AUTH_SOCK nix/shell.nix
```

You should then be able to build WaterPaths as described above.

## Using Containers

The containers enompass the nix instructions described above, but use
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
mpirun -n 4 triangleSimulation -T 4 -t 2344 -r 16 -d /home/nixuser/WaterPaths/ -I _few_records -s decvars.csv -m 0 -b t
rue
```

Note that we use `-n 4`: `-n 5` would not work since we only have 4 nodes (really, containers): 3 nodes plus 1 head node.

Now you should be able to follow the "Running WaterPaths" directions above.

### Singularity

(Work in progress)