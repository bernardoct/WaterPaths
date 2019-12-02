## Build JLWTP Agreement Model Parameter Input file
##  Nov 2019
## ------------------------------------------------

rm(list=ls())
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/WaterPaths")

## List of decision parameters, reasonable min/max ranges if not fixed --------
## by utility -----------------------------------------------------------------

# Cary
Cary_Restriction_ROF_Trigger_Range = c(0.01,0.25)
Cary_JordanLake_Allocation = c(0.40) # permitted sum capacity of apex, cary, morrisville, RTP current JL allocation (fraction of 100 MGD)
Cary_ContingencyFund_AnnualPayment_Range = c(0,0.2) # as a fraction of annual revenues
Cary_ContingencyFund_Cap_Range = c(0,100) # in $MM
Cary_Insurance_FixedPayout_Range = c(0,10) # in $MM
Cary_Insurance_ROF_Trigger_Range = c(0.01,0.25)
Cary_Infrastructure_ROF_Trigger_Range = c(0.01,0.25)
Cary_Infrastructure_DemandBuffer_Range = c(0,10) # in MGD
Cary_DemandProjection_ForecastLength = c(5) # years
Cary_DemandProjection_HistoricalLookBack = c(5) # years
Cary_DemandProjection_ReprojectionFrequency = c(5) # years

# Durham
Durham_Restriction_ROF_Trigger_Range = c(0.01,0.25)
Durham_Transfer_ROF_Trigger_Range = c(0.01,0.25)
Durham_JordanLake_Allocation = c(0.10) # current but not future expected in all cases (0.16)
Durham_ContingencyFund_AnnualPayment_Range = c(0,0.2) # as a fraction of annual revenues
Durham_ContingencyFund_Cap_Range = c(0,100) # in $MM
Durham_Insurance_FixedPayout_Range = c(0,10) # in $MM
Durham_Insurance_ROF_Trigger_Range = c(0.01,0.25)
Durham_Infrastructure_ROF_Trigger_Range = c(0.01,0.25)
Durham_Infrastructure_DemandBuffer_Range = c(0,10) # in MGD
Durham_DemandProjection_ForecastLength = c(5) # years
Durham_DemandProjection_HistoricalLookBack = c(5) # years
Durham_DemandProjection_ReprojectionFrequency = c(5) # years
Durham_SharedJLWTP_TreatmentAllocation = c(0.01,0.5) # fraction of total capacity

# OWASA
OWASA_Restriction_ROF_Trigger_Range = c(0.01,0.25)
OWASA_Transfer_ROF_Trigger_Range = c(0.01,0.25)
OWASA_JordanLake_Allocation = c(0.05) # future and expected
OWASA_ContingencyFund_AnnualPayment_Range = c(0,0.2) # as a fraction of annual revenues
OWASA_ContingencyFund_Cap_Range = c(0,100) # in $MM
OWASA_Insurance_FixedPayout_Range = c(0,10) # in $MM
OWASA_Insurance_ROF_Trigger_Range = c(0.01,0.25)
OWASA_Infrastructure_ROF_Trigger_Range = c(0.01,0.25)
OWASA_Infrastructure_DemandBuffer_Range = c(0,10) # in MGD
OWASA_DemandProjection_ForecastLength = c(5) # years
OWASA_DemandProjection_HistoricalLookBack = c(5) # years
OWASA_DemandProjection_ReprojectionFrequency = c(5) # years
OWASA_SharedJLWTP_TreatmentAllocation = c(0.01,0.2) # fraction of total capacity

# Raleigh
Raleigh_Restriction_ROF_Trigger_Range = c(0.01,0.25)
Raleigh_Transfer_ROF_Trigger_Range = c(0.01,0.25)
Raleigh_JordanLake_Allocation = c(0) # current and future expected
Raleigh_ContingencyFund_AnnualPayment_Range = c(0,0.2) # as a fraction of annual revenues
Raleigh_ContingencyFund_Cap_Range = c(0,100) # in $MM
Raleigh_Insurance_FixedPayout_Range = c(0,10) # in $MM
Raleigh_Insurance_ROF_Trigger_Range = c(0.01,0.25)
Raleigh_Infrastructure_ROF_Trigger_Range = c(0.01,0.25)
Raleigh_Infrastructure_DemandBuffer_Range = c(0,10) # in MGD
Raleigh_DemandProjection_ForecastLength = c(5) # years
Raleigh_DemandProjection_HistoricalLookBack = c(5) # years
Raleigh_DemandProjection_ReprojectionFrequency = c(5) # years
Raleigh_SharedJLWTP_TreatmentAllocation = c(0) # fraction of total capacity
Raleigh_FallsLakeReallocation_Quantity = c(14000) # in MG, need to check this value

# Chatham County
Chatham_Restriction_ROF_Trigger_Range = c(0.01,0.25)
Chatham_Transfer_ROF_Trigger_Range = c(0.01,0.25)
Chatham_JordanLake_Allocation = c(0.06) # based on current levels if the total pool size is 100 MGD
Chatham_ContingencyFund_AnnualPayment_Range = c(0,0.2) # as a fraction of annual revenues
Chatham_ContingencyFund_Cap_Range = c(0,100) # in $MM
Chatham_Insurance_FixedPayout_Range = c(0,10) # in $MM
Chatham_Insurance_ROF_Trigger_Range = c(0.01,0.25)
Chatham_Infrastructure_ROF_Trigger_Range = c(0.01,0.25)
Chatham_Infrastructure_DemandBuffer_Range = c(0,10) # in MGD
Chatham_DemandProjection_ForecastLength = c(5) # years
Chatham_DemandProjection_HistoricalLookBack = c(5) # years
Chatham_DemandProjection_ReprojectionFrequency = c(5) # years
Chatham_SharedJLWTP_TreatmentAllocation = c(0.01,0.4) # fraction of total capacity

# Pittsboro
Pittsboro_Restriction_ROF_Trigger_Range = c(0.01,0.25)
Pittsboro_Transfer_ROF_Trigger_Range = c(0.01,0.25)
Pittsboro_JordanLake_Allocation = c(0.06) # current is 0, this is Round 4/2060 expected level
Pittsboro_ContingencyFund_AnnualPayment_Range = c(0,0.2) # as a fraction of annual revenues
Pittsboro_ContingencyFund_Cap_Range = c(0,100) # in $MM
Pittsboro_Insurance_FixedPayout_Range = c(0,10) # in $MM
Pittsboro_Insurance_ROF_Trigger_Range = c(0.01,0.25)
Pittsboro_Infrastructure_ROF_Trigger_Range = c(0.01,0.25)
Pittsboro_Infrastructure_DemandBuffer_Range = c(0,10) # in MGD
Pittsboro_DemandProjection_ForecastLength = c(5) # years
Pittsboro_DemandProjection_HistoricalLookBack = c(5) # years
Pittsboro_DemandProjection_ReprojectionFrequency = c(5) # years
Pittsboro_SharedJLWTP_TreatmentAllocation = c(0.01,0.3) # fraction of total capacity

## Infrastructure pathway/sequencing parameters organized by utility ---------

# Cary
Cary_WTPUpgrade_1 = c(0.01,0.99) # this project never actually gets used anymore
Cary_WTPUpgrade_2 = c(0.01,0.99)
Cary_WTPUpgrade_3 = c(0.01,0.99)

# Durham
Durham_TeerQuarry_Expansion = c(0.01,0.99)
Durham_LakeMichie_Expansion_Low = c(0.01,0.99)
Durham_LakeMichie_Expansion_High = c(0.01,0.99)
Durham_ReclaimedWater_Expansion_Low = c(0.01,0.99)
Durham_ReclaimedWater_Expansion_High = c(0.01,0.99)
Durham_SharedJLWTP_Low = c(0.01,0.99)
Durham_SharedJLWTP_High = c(0.01,0.99)

# OWASA
OWASA_UniversityLake_Expansion = c(0.01,0.99)
OWASA_CaneCreekResevroir_Expansion = c(0.01,0.99)
OWASA_StoneQuarry_Expansion_Low = c(0.01,0.99)
OWASA_StoneQuarry_Expansion_High = c(0.01,0.99)
OWASA_SharedJLWTP_Low = c(0.01,0.99)
OWASA_SharedJLWTP_High = c(0.01,0.99)

# Raleigh
Raleigh_LittleRiverReservoir = c(0.01,0.99)
Raleigh_RichlandQuarry = c(0.01,0.99)
Raleigh_NeuseRiverIntake = c(0.01,0.99)
Raleigh_FallsLakeReallocation = c(0.01,0.99)
Raleigh_SharedJLWTP_Low = c(0.0) # raleigh isn't really a part of this, it's an artifact of old modeling
Raleigh_SharedJLWTP_High = c(0.0)

# Chatham County
Chatham_CapeFearIntake = c(0.01,0.99)
Chatham_SharedJLWTP_Low = c(0.01,0.99)
Chatham_SharedJLWTP_High = c(0.01,0.99)

# Pittsboro
Pittsboro_HawRiverIntake_Expansion_Low = c(0.01,0.99)
Pittsboro_HawRiverIntake_Expansion_High = c(0.01,0.99)
Pittsboro_SharedJLWTP_Low = c(0.01,0.99)
Pittsboro_SharedJLWTP_High = c(0.01,0.99)

## List of all parameter names ------------------------------------------
all_param_names = ls()
# double-check this, links each parameter (based on their order in ls()) to the order they must be in for
# the triangle parameter input file (ls should correct R variables in memory to alphabetical order)
# the indices in this vector are direct from Triangle.cpp and a +1 is added to the vector to correct for
# C++ indexing from 0 and R indexing from 1
column_indices_for_each_parameter = c(14,82,88,89,90,56,26,22,18,10,3,46,47,48, # Cary, 14 params
                                      81,68,86,100,101,102,72,71,70,69,67,65,78,77,74,66, # Chatham, 16 params
                                      11,83,91,92,93,53,23,19,15,9,35,34,33,32,0,43,42,50,31,4, # Durham, 20 params
                                      28,12,84,94,95,96,54,24,20,16,7,1,41,40,49,30,29,5,27, # OWASA, 19 params
                                      60,87,103,104,105,80,79,64,63,62,61,59,57,76,75,73,58, # Pittsboro, 17 params
                                      13,85,97,98,99,39,52,55,25,21,17,8,36,38,2,37,45,44,51,6) + 1 # Raleigh, 20 params

## Build sets of parameters for 1,000 simulations -----------------------
parameter_sets = matrix(NA,1000,length(all_param_names))

# build many parameter sets based on ranges of values
for (pset in 1:1000) {
  i = 1
  for (param in all_param_names) {
    #print(param)
    if (length(get(param)) == 1) {
      parameter_sets[pset,column_indices_for_each_parameter[i]] = get(param)
    } else {
      parameter_sets[pset,column_indices_for_each_parameter[i]] = runif(1,min = get(param)[1], max = get(param)[2])
    }
    i = i + 1
  }
}

write.table(parameter_sets, "parameter_sets_1000_solutions.csv", sep = ",", row.names = F, col.names = F)


## Build input file that varies specific parameters and holds all others steady ----------------
# names need to match exactly with names within the all_parameter_names vector
# and get their associated index
params_to_vary = c("Pittsboro_SharedJLWTP_TreatmentAllocation", 
                   "Chatham_SharedJLWTP_TreatmentAllocation", 
                   "Durham_SharedJLWTP_TreatmentAllocation", 
                   "OWASA_SharedJLWTP_TreatmentAllocation")

param_input_file_index = c(); param_ranges = matrix(NA, 4, length(params_to_vary)); pi = 1
for (p in params_to_vary) {
  param_input_file_index = c(param_input_file_index,
                             column_indices_for_each_parameter[which(all_param_names == p)])
  if (length(get(p)) == 1) {print("Break the loop, this parameter needs a range to vary it."); break} else {
    param_ranges[,pi] = seq(get(p)[1], get(p)[2], length.out = ncol(param_ranges))
  }
  pi = pi + 1
}

library(tidyverse)
p_r = as.data.frame(param_ranges)
p_ranges_expanded = expand(p_r,V1,V2,V3,V4)
colnames(p_ranges_expanded) = params_to_vary

parameter_sets = matrix(NA,nrow(p_ranges_expanded),length(all_param_names))

# build many parameter sets based on ranges of values
i = 1
for (param in all_param_names) {
  if (param %in% params_to_vary) {
    full_range_of_param = as.vector(as.matrix(p_ranges_expanded[,which(params_to_vary == param)]))
    parameter_sets[,column_indices_for_each_parameter[i]] = full_range_of_param
  } else {
    if (length(get(param)) == 1) {
      parameter_sets[,column_indices_for_each_parameter[i]] = get(param)
    } else {
      parameter_sets[,column_indices_for_each_parameter[i]] = runif(1,min = get(param)[1], max = get(param)[2])
    }
  }

  i = i + 1
}

write.table(parameter_sets, "parameter_sets_specific_params_varied.csv", 
            sep = ",", row.names = F, col.names = F)
















