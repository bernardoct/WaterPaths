# WaterPaths
A utility planning and management tool based off the NC Triangle model.

## What is included in this repository
- Source code (`/src`).
- NC Triangle water planning and management problem (`src/Problem/PaperTestProblem.cpp`).
- NC Triangle data (`/TestFiles`).
- Sample input time series (`/TestFiles/inflows`, `evaporation`, `demand`) with 64 realizations each.

## Compiling WaterPaths
Use Makefile provided in source directory. E.g., if using GCC run 
```
make gcc
```

## Running WaterPaths
To print a list of flags to for running WaterPaths, use 
```
./waterpaths -?
```

### Running WaterPaths in simulation mode
For running a reduced version of the NC Triangle model in full simulation mode, run:
``` 
./waterpaths -T 4 -t 2344 -r 64 -d /your/current/path/ -s sample_solutions.csv -m 0 
```
The argument of flag -T is the number of threads to be created using OpenMP. If -T flag is not used, the 64 realizations will be distributed across all physical and virtual cores.

There are three options for decreasing runtime for testing purposes that can be used individually or together:
- Decreasing the number of realizations (-r flag)
- Decreasing the simulation time (-t flag) to no less than 53 (weeks)
- Generating pre-computed risk-of-failure (ROF) tables to be used during runs. Keep in mind that if using a different set of inflow, evaporation, of RDM files will warrant new tables. To generate tables, run 
```
./triangleSimulation -T 4 -t 2344 -r 64 -d /your/current/path/ -s sample_solutions.csv -m 0 -C 1
```
 To run a function evaluation using the tables, set value of -C flag to -1.

### Runnning WaterPaths with Borg MS in optimization mode
To get a copy of Borg, go to Borg's [official website](http://borgmoea.org/) and request access to the source  code (free for non-commercial use). You should soon after get an e-mail with a link to its repository, where you can download the source code from.

To run WaterPaths in optimization mode, WaterPaths needs to be re-compiled with Borg and the appropriate flags must be passed when calling it from the command line with a program like mpirun. To do so, follow the following steps:
1. In order to run WaterPaths with Borg, the library libborgms.a must be provided in the /lib folder. To do so, borgmoea.org/ and request a licence, move Borg files to the folder /borg, compile borg by running `make mpi` in /borg, and finally move the file libborgms.a to the /lib folder.
2. After libborgms.a is compiled and place in /lib, compile WaterPaths with `make borg`.
3. Copy directories cube:/scratch/bct52/TestFiles and cube:/scratch/bct52/rof_tables_cac/ to your directory of preference, or 
Define a `${DATA_DIR}` that includes a `rof_tables_cac/` directory and the `TestFiles/` directory. 
4. Run WaterPaths with 
```
mpiexec --hostfile nodefile -np 3 -x OMP_NUM_THREADS ./waterpaths -T 5 -t 2344 -r ${N_REALIZATIONS} -d ${DATA_DIR} -C -1 -O ${DATA_DIR}rof_tables_cac/ -U TestFiles/rdm_utilities_reeval.csv -P TestFiles/rdm_dmp_reeval.csv -W TestFiles/rdm_water_sources_reeval.csv -b true -n 202 -o 100 -e 1
```
 The WaterPaths call above is optimized for a node with 16 cores and will spawn 3 MPI processes running on 5 threats each. Be sure to change the nodefile (see single_run_borgms.sh) and the value of the -T flag for nodes with different number of cores. 
5. The run should take less than 10 minutes if 16 or more cores are being used. The two output files of this run should now be in `${DATA_DIR}/output/`. The NC_output_MM_S1_N202.set file should consist on a space-separated matrix with 20-80 rows by 63 columns. The /scratch/bct52/output/NC_runtime_MM_S1_N202.runtime file should have between 90 and 200 lines.

# Using the InputFileParser
To simulate and optimize a custom water resource system, the InputFileParser provides front-end support.

## Structure
To create your own model in WaterPaths, generate a plain-text file (.wp extension), following the format of the files in the `Tests` directory and described in detail below.

### Blocks
WaterPaths looks for ***blocks*** of information, which are delimited by *tags* and describe information on *parameters*.
A ***block*** follows the following form:

```
[TAG]
parameter value
parameter value
...
```

where a [TAG] is an upper-case keyword (defined below) enclosed in brackets and each *parameter* with a corresponding *value*.

### Values
The rest of this documentation will refer to value types as follows:

| Value Type    | Description                                                             |
|---------------|-------------------------------------------------------------------------|
| int           | Standard integer (e.g. 1)                                               |
| double        | Standard double-precision value (e.g. 1.05)                             |
| string        | Standard string                                                         |
| csv           | Path to comma-separated values file                                     |
| ref           | Path to .reference file                                                 |
| dir           | Path to a directory                                                     |
| Utility       | Name of a declared utility.                                             |
| Source        | Name of a declared water source (reservoir, allocated reservoir, reuse) |
| type,type     | Two values of type separated by commas                                  |
| type,type,... | One or more values of type separated by commas                          |
| type type     | Two values of type separated by spaces                                  |
| type type ... | One or more values of type separated by spaces                          |

Additionally, the type `bond info` can be represented as 
```
level int int int int deferred
```
which corresponds to 
```
type cost_of_capital n_payments coupon_rate pay_on_weeks begin_repayment_at_issuance
```
Currently, `level` is the only bond type available and `deferred` is the only repayment setting.

### Tags
The currently implemented input file parser [TAG]s are described below.

| Tag                                        | Description                                                       |
|--------------------------------------------|-------------------------------------------------------------------|
| RUN PARAMETERS                             | Information on the type and size of a WaterPaths simulation.      |
| DATA TO LOAD                               | Location of price, demand, inflow, and evaporation files.         |
| RESERVOIR                                  | Defines an existing or potential reservoir.                       |
| ALLOCATED RESERVOIR                        | Defines an existing or potential reservoir allocated between utilities.|
| RESERVOIR EXPANSION                        | Defines an infrastructure expansion for a reservoir.    |
| WATER REUSE                                | Defines a water reuse development project.                        |
| UTILITY                                    | Defines a basic water utility.                                 |
| WATER SOURCES GRAPH                        | Specify the connections between water sources.                    |
| WS TO UTILITY MATRIX                       | Set up the water source - utility connectivity matrix.      |
| UTILITIES GRAPH                            | Specify the connections between utilities.                         |
| TABLE STORAGE SHIFT                        | Defines the impact of all infrastructure expansion projects on pre-calculated ROF tables.      |
| RESTRICTIONS POLICY                        | Defines a short-term ROF water use restrictions policy for a utility.   |
| TRANSFERS POLICY                           | Defines a short-term ROF water transfer policy between mutiple utilities. |
| INSURANCE POLICY                           | Defines a short-term ROF insurance policy for a utility. |
| FIXED FLOW RESERVOIR CONTROL RULE          | A reservoir operating policy that dictates a fixed release.                |
| INFLOW-BASED RESERVOIR CONTROL RULE        | A reservoir operating policy where release is dependent on inflow.       |
| SEASONAL RESERVOIR CONTROL RULE            | A reservoir operating policy that varies seasonally.   |
| DECISION VARIABLE BOUNDS                   | Declaration of decision variables and their bounds for optimization runs.   |
| OBJECTIVES EPSILONS                        | Epsilon (resolution) for objective values in optimization runs.   |

### [RUN PARAMETERS]

|        Parameter       |        Value Type        | Description                                                                             |
|:-----------------------|:-------------------------|-----------------------------------------------------------------------------------------|
| n_realizations         |            int           | Number of realizations                                                                  |
| n_weeks                |            int           | Number of weeks                                                                         |
| rdm_utilities          |            csv           | Input file that contains sampled deeply-uncertain parameters that effect water utilities. <br/> Each column represents a different factor, and each row represents a state of the world.  |
| rdm_water_sources      |            csv           | Same as rdm_utilities, but for water sources. |
| rdm_dmps               |            csv           | Same as rdm_utilities, but for drought management policies.  |
| rdm_no                 |            int           | RDM sample to run (only use is running one SOW) |
| n_threads              |            int           | Number of threads (should not exceed twice the number of core available)              |
| rof_tables_dir         |            dir           | Directory to export or import risk-of-failure metric table                              |
| use_rof_tables         | "generate"<br/>"import"<br/>"no" | Generate ROF table<br/>Import ROF table for speedup<br/>Neither                                 |
| print_time_series      |           bool           | If present, print the time series data                                                  |
| realizations_to_run    |           int,int        | Range of realizations to run.                                                        |
| solutions_file         |            ref           | Location of solutions to run                                                            |
| n_bootstrap_samples    |            int           | Number of bootstrap samples                                                             |
| bootstrap_sample_size  |            int           | Size of bootstrap samples                                                               |
| solutions_to_run       |            int           | Number of solutions to run                                                              |
| solutions_to_run_range |          int,int         | Range of solutions to run, (e.g. "3,20" would run solutions: 3, 4, ..., 20)             |
| seed                   |            int           | Seed for random number generation                                                       |
| optimize               |          int int         | If present, optimize with Borg for max evaluations and output frequency, respectively |

### [DATA TO LOAD]
To load *csv* data needed for a WaterPaths simulation, use
```
filename csv
```
where `filename` is a unique name used to reference the csv file at `csv` in the remainder of the input file. <br/>
To load only the first `n_realizations` lines of the file, use `*` before the filename:
```
*filename csv
```
Without `*`, the entire file will be loaded.

### [RESERVOIR]
| Parameter          | Value Type                              | Description                                                    |
|:-------------------|:----------------------------------------|:---------------------------------------------------------------|
| name               | string                                  | Name of this reservoir (to be referred thereafter)             |
| capacity           | int                                     | Reservoir capacity.                           |
| treatment_capacity | double                                  | Reservoir treatment capacity.                 |
| streamflow_files   | filename filename ... | Loaded files describing the streamflow leaving this reservoir. |
| evaporation_file   | filename                                | Loaded file with evaporation realizations of this reservoir.   |
| storage_area       | int                                     | Reservoir area.                            |
| storage_area_curve | int,int int,int ...                     | Pair-wise relationship defining reservoir storage to reservoir area.            |
| bond               | bond info                               | Bond for an infrastructure expansion.                          |
| ctime              | int int                                 | Construction time range: low-high, respectively, in years.     |
| ptime              | int                                     | Permitting time for infrastructure expansion, in years.        |
| utilities_with_allocations             | int                                     | Permitting time for infrastructure expansion, in years.        |

### [ALLOCATED RESERVOIR]
Same parameters as [RESERVOIR], in addition to:

| Parameter                     | Value Type          | Description                                                                                                                    |
|:------------------------------|:--------------------|:-------------------------------------------------------------------------------------------------------------------------------|
| utilities_with_allocations    | Utility,Utility,... | Comma separated list of utilities with allocations on this reservoir                                                           |
| allocated_fractions | double,double,...   | Comma separated list of fractions (< 1) corresponding to each <br/> utility's allocation, in the order of utilities_with_allocations |
| allocated_treatment_fractions | double,double,..| Comma seperated list of fractions corresponding to each <br/> utility's treatment allocations.              |

### [RESERVOIR EXPANSION]
Define a [RESERVOIR] or [ALLOCATED RESERVOIR] with parameters representing values after expansion. <br/>
`ctime` and `ptime` represent the expansion duration.<br/> 
```
parent_reservoir Reservoir
```
is the reservoir the expansion is performed on.

### [WATER REUSE]
Same parameters as [RESERVOIR]. Must include,
```
treatment_capacity (double)
```

### [UTILITY]
| Parameter                             | Value Type                       | Description                                                          |
|---------------------------------------|----------------------------------|----------------------------------------------------------------------|
| name                                  | string                           | Name of this utility                                                 |
| demands                               | filename                         | File with demand data                                                |
| number_of_week_demands                | int                              |  ??                                                                    |
| percent_contingency_fund_contribution | double                           | Percent of anual volumentric revenue that is contributed to a reserve fund.        |
| typesMonthlyDemandFraction            | filename                         | ?? |
| typesMonthlyWaterPrice                | filename                         | Corresponding water price for each category.  |
| wwtp_discharge_rule                   | filename Source,Source,... | Rule dictating wastewater return flow.    |
| demand_buffer                         | double                           | Demand buffer for this utility                                       |
| rof_intra_construction_order          | Source,Source,...          | Order of precedence for infrastructure expansion                     |
| infra_construction_triggers           | double,double,...                | ROF triggers values for infrastructure expansions, in order as above |
| infra_discount_rate                   | double                           | Discount rate for infrastructure expansions.                         |
| water_source_to_wtp                   | Source Source ...          | Water source to water treatment plant connection.  |
| utility_owned_wtp_capacities          | double,double,...                | Capacities of each water treatment plant connected to the utility.    |
| demand_infra_construction_order       | Source,Source,...          | Order of infrastructure construction, triggered by demand.        |
| infra_if_built_remove                 | Source Source ...                             | List of mutually exclusive infrastructure options that must be removed if another option is triggered. |
| construction_pre_requesites           | Source Source ...                     | List of infrastructure options that must be triggered prior to this |

### [WATER SOURCES GRAPH]
This tag creates the network of a water resources system as a graph, with edges corresponding to connections between reservoirs, allocated reservoirs, and water reuses. <br/> 
There are no parameters in this block; instead, each line represents a directed *edge* in the water source graph. Each entry must be the `name` of a declared water source.<br/> 
If a connection exists between `source1` and `source2`, include a line in this block as such:
```
source1,source2
```
For example, this InputFileParser graph and its visual map are shown.
```
source1,source2
source2,source3
source4,source5
source5,source3
```
![](https://user-images.githubusercontent.com/61888627/85165928-8e14e900-b234-11ea-9161-6c9b2e14a01c.png)

### [UTILITIES GRAPH]
This tag is similar to [WATER SOURCES GRAPH], except for utilites. An *edge* between water sources represents a potential water exchange between two utilites. The same format as [WATER SOURCES GRAPH] is followed.

### [WS TO UTILITY MATRIX]
WaterPaths represents the connections between utilities and water sources as
```
utility source1,source2,...
```
where `utility` is a utillity name and the subsequent `sources` are water source names. Here, `utility` uses `source1`, `source2`, and all following sources to reach its demand. Each declared utility must have its own line in this block.

### [TABLE STORAGE SHIFT]

Here, we define a matrix for respective table shortage shifts for potential reservoir expansions.<br/>
Entries should be in the following form:
```
Utility Reservoir shift
```
where `shift` is the integer representing associated reservoir table shift. 

### [RESTRICTIONS POLICY]
| Parameter                  | Value Type          | Description                                                              |
|----------------------------|---------------------|--------------------------------------------------------------------------|
| apply_to_utilities         | Utility,Utility,... | Which utility (or utilities) this restriction policy is to be applied to |
| stage_multipliers          | double,double,...   | Demand reductions (percentage of unrestricted demand) associated with each restriction stage. |
| stage_triggers             | double,double,...   | Short term ROF restrictions triggers                                     |
| typesMonthlyDemandFraction | filename            | ?? |
| typesMonthlyWaterPrice     | filename            | ?? |
| priceMultipliers           | filename            | ?? |

### [TRANSFERS POLICY]
| Parameter                | Value Type          | Description                                                      |
|--------------------------|---------------------|------------------------------------------------------------------|
| apply_to_utilities       | Utility,Utility,... | The utility (or utilities) that are the buyers for this transfer |
| source_utility_id        | Utility             | Which utility is the source for this transfer policy             |
| transfer_water_source_id | Source              | Which source the transfers utilize                               |
| source_treatment_buffer  | double              | ?? |
| pipe_transfer_capacities | double,double,...   | Capacity of inter-connections between utilities. |
| buyers_transfer_triggers | double,double,...   | Short term ROF triggers for transfers                            |

### [INSURANCE POLICY]
| Parameter          | Value Type          | Description                                                      |
|--------------------|---------------------|------------------------------------------------------------------|
| apply_to_utilities | Utility,Utility,... | The utility (or utilities) that are the buyers for this transfer |
| insurance_triggers | double,double,...   | Short term ROF triggers for this drought insurance policy        |
| insurance_premium  | double              | The premium on this insurance policy                             |
| fixed_payouts      | double,double,...   | ?? |

### [FIXED FLOW RESERVOIR CONTROL RULE]
| Parameter            | Value Type          | Description                                |
|----------------------|---------------------|--------------------------------------------|
| water_source_id      | Source              | The source of this fixed flow control rule |
| release              | int                 | Volume of water released. |
| aux_water_sources_id | Source,Source,...   | Auxiliary sources for this control rule    |
| aux_utilities_id     | Utility,Utility,... | Auxiliary utilities for this control rule    |

### [INFLOW-BASED RESERVOIR CONTROL RULE]
| Parameter            | Value Type          | Description                                |
|----------------------|---------------------|--------------------------------------------|
| water_source_id      | Source              | The source of this inflow-based control rule |
| inflows              | double,double,...   | Vector of reservoir inflows that define releases |
| releases             | double,double,...   | Vector of released defined by inflows |
| aux_water_sources_id | Source,Source,...   | Auxiliary sources for this control rule    |
| aux_utilities_id     | Utility,Utility,... | Auxiliary utilities for this control rule    |

### [SEASONAL RESERVOIR CONTROL RULE]
| Parameter            | Value Type          | Description                                |
|----------------------|---------------------|--------------------------------------------|
| water_source_id      | Source              | The source of this fixed flow control rule |
| week_thresholds      | int,int,int,int  | Weeks that define each season |
| releases             | double,double,double,double   | Releases for each season |
| aux_water_sources_id | Source,Source,...   | Auxiliary sources for this control rule    |
| aux_utilities_id     | Utility,Utility,... | Auxiliary utilities for this control rule    |

### [DECISON VARIABLE BOUNDS]
If this WaterPaths simulation is an optimization, this block tells the BorgMOEA which variables to optimize within the WaterPaths framework. Each line should follow this format:
```
index lowerbound,upperbound
```
The `lowerbound` and `upperbound` numeric values indicate the lowest and highest values this decision variable can take. <br/>
The `index` value is an integer, and each value in *0..n-1*, where *n* is the number of decision varaibles, must be taken. <br/>

A decision variable's `index` describes its relative ordered position within the *.wp* file. To declare a specific decision variable instance in another block, use `%%%` for optimized *values* and `@` preceding an alias for optimized orderings. <br/>

For example, consider a simple optimization case with 4 decision variables.<br/>

1. Declare the decision varibles in a [DECISION VARIABLE BOUNDS] block.
```
[DECISION VARIABLE BOUNDS]
0 0.001,0.75    # utility1 transfer ROF trigger
1 0.0,1,0       # source1 construction rank
2 0.0,1.0       # source2 construction rank
3 0.0,1.0       # source3 construction rank
```

2. Declare decision variable `0` (utility1 transfer ROF trigger) within a [TRANSFERS POLICY] block with a `%%%`
```
[TRANSFERS POLICY]
apply_to_utilities utility1
source_utility_id RandomUtility
...
buyers_transfer_triggers %%%
```

3. Declare decison variables `1` `2` and `3` (construction ranks) within a [UTILITY] block with a series of `@`
```
[UTILITY]
name RandomUtlity
...
rof_infra_construction_order @source1,@source2,@source3
...
```

This input file configuration will optimize a short-term transfer ROF for `utility1` and a long-term infrastructure expansion ordering for each `source`. Ensure that the intended ordering of decision varaibles (as declared within the [DECISION VARIABLES BOUNDS] block) corresponds to the order of appearence in the remainder of the input file. <br/>

For a more advanced optimization problem formulation, refer to `Tests/test_input_file_borg.wp`. 

### [OBJECTIVES EPSILONS]
This tag declares the epsilon values for the five objectives:
1. Reliability
2. Restriction Frequency
3. Infrastructure NPV
4. Financial Cost
5. Worse First Percentile Cost

This block has one line, in the form:
```
ep1,ep2,ep3,ep4,ep5
```
where each epsilon corresponds to the respective numbered objective.<br/>

To use the recommended epsilon values, include this block:
```
[OBJECTIVES EPSILONS]
0.001,0.02,10.0,0.025,0.01
```
