## WaterPaths Bootstrapping Comparisons for WJLWTP Model
## Choose single bootstrap sample and build new input files
##    only containing those realizations
##
##  Sept 2020
## ------------------------------------------------

rm(list=ls())
setwd('C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/WaterPaths/TestFiles')

#############################################################
#############################################################
#############################################################

# BEFORE RUNNING MODEL WITH THE FILES MADE BELOW
# MUST REMAKE ROF TABLES

#############################################################
#############################################################
#############################################################

# choose random set of 500 realizations (of 1000)
# ensuring no duplication of realizations
r_set = sample(1:1000, size = 500, replace = FALSE)

# develop matching input files with just these realizations
# and make a record of the realization IDs used
write.table(r_set, "bootstrap_realization_IDs.csv", 
            row.names = FALSE, col.names = FALSE, sep = ",")

# PLACE THESE FILES IN THE "inflows" DIRECTORY FOR ACTUAL RUNS
# THEY ARE JUST DUMPED TO THIS FOLDER FOR RECORD-KEEPING ALSO
inflow_files = list.files("inflows", pattern = "*.csv")
for (site in inflow_files) {
  I = read.csv(paste("inflows", site, sep = "/"), header = FALSE)
  write.table(I[r_set,], paste("inflows/bootstrap_inflows/", site, sep = ""), 
              row.names = FALSE, col.names = FALSE, sep = ",")
}
rm(I)

# LIKE ABOVE, PLACE THESE IN THE "demands/sinusoidal" FOLDER TO RUN
demand_files = list.files("demands/sinusoidal", pattern = "*syn*")
for (site in demand_files) {
  D = read.csv(paste("demands/sinusoidal", site, sep = "/"), header = FALSE)
  write.table(D[r_set,], paste("demands/bootstrap_demands/", site, sep = ""), 
              row.names = FALSE, col.names = FALSE, sep = ",")
}
rm(D)

# PLACE THESE FILES IN THE "evaporation" DIRECTORY FOR ACTUAL RUNS
# THEY ARE JUST DUMPED TO THIS FOLDER FOR RECORD-KEEPING ALSO
evap_files = list.files("evaporation", pattern = "*.csv")
for (site in evap_files) {
  E = read.csv(paste("evaporation", site, sep = "/"), header = FALSE)
  write.table(E[r_set,], paste("evaporation/bootstrap_evaporation/", site, sep = ""), 
              row.names = FALSE, col.names = FALSE, sep = ",")
}
rm(E)

# NAME OF THESE FILES CHANGES, BE AWARE
rdm_files = list.files(pattern = "WJLWTP*")
for (r in rdm_files) {
  if (length(strsplit(r, "_")[[1]]) == 3) {
    RDM = read.csv(r, header = FALSE)
    write.table(RDM[r_set,], paste("bootstrap", r, sep = "_"), 
                row.names = FALSE, col.names = FALSE, sep = ",")
  }
}




