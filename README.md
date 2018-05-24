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
`./triangleSimulation -T 4 -t 2344 -r 88 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -I _few_records -s decvars.csv -m 0`.
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
3. Run WaterPaths with `mpirun -n 5 triangleSimulation -T 4 -t 2344 -r 88 -d /mnt/c/Users/Bernardo/CLionProjects/WaterPaths/ -I _few_records -s decvars.csv -m 0 -b true`. Keep in mind that the number of MPI processes must be at least 3 but the minimum recommended is 5. also keep in mind that each MPI process will try to create the number of threads specified with flag -T.
