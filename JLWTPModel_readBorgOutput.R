## Read Borg Output for Six Utility Model
##  Mar 2020
## ------------------------------------------------

rm(list=ls())
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

### ------------------------------------------------------------------------------------------------------
# Oct 2020: Make csv solution file from borg satisficing reference set
Sols = read.csv("combined_satisficing_pareto_front_by_formulation_withDVs_NFE50000.csv")
DVs = Sols[,8:ncol(Sols)]
write.table(DVs, "satisficing_sols_DVs_NFE50000.csv", row.names = FALSE, col.names = FALSE, sep = ",")

# Pull demand multipliers to find top/bottom 100 realizations in terms of demand growth for tests
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/RealizationGeneration")
sinusoidal_multiplier = function(time, A, Tau, p) {return(1 + A * sin(2 * pi * time / Tau + p) - A * sin(p))}

# read sinusoidal parameter set and get demands of last 10 years
LHS_parameter_samples = as.matrix(read.table("RDMfactor_sets/demand_factors.csv", sep = ",", header = FALSE))
weeks = (2496-52*10):2496; n_reals = 500
multipliers = matrix(NA, nrow = n_reals, ncol = length(weeks))
for (r in 1:n_reals) {
  for (w in weeks) {
    multipliers[r,(w-weeks[1]+1)] = as.numeric(as.character(sinusoidal_multiplier(time = w, 
                                                                     A = LHS_parameter_samples[r,1], 
                                                                     Tau = LHS_parameter_samples[r,2], 
                                                                     p = LHS_parameter_samples[r,3])))
  }
}

# find realizations with greatest average values across final 10 years
real_avg = apply(multipliers, MARGIN = 1, mean); real_id = 0:499
real_tops = real_id[real_avg > 1.082]
real_lows = real_id[real_avg < 0.927]
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/WaterPaths/TestFiles")
write.table(rbind(real_tops, real_lows), "bootstrapped_100_reals_highlowdemand.csv", 
            sep = ",", col.names = FALSE, row.names = FALSE)

### ------------------------------------------------------------------------------------------------------

# read formulations 0, 1, and 2
# datasets not same size so use list to hold
formulations = c("S0", "S1", "S2")
BorgOut = list(); i = 1
for (f in formulations) {
  BorgOut[[i]] = read.table(
    paste("NC_output_MS_", f, "_N50000.set", sep = ""), 
    sep = " ", header = FALSE)
  i = i + 1
}

# first 69 cols are DVs, last 6 Objectives
# pull objectives here and organize for using
# Dave's online parallel plot tool with 'group' col
# https://dgoldri25.github.io/Categorical-ParallelCoordinatePlot/
FormulationNames = c("No Agreement", 
                     "Fixed Capacity Agreement",
                     "Adjustible Capacity Agreement")
ObjectiveNames = c("Reliability", 
                   "Restriction Use", 
                   "Infrastructure NPC", 
                   "Peak Financial Cost",
                   "Worst-Case Mitigation Cost",
                   "Unit Cost of Expansion", "group")
ObjectiveResultsToPlot = c()
for (f in c(0,1,2)) {
  ObjectiveResultsToPlot = 
    rbind(ObjectiveResultsToPlot, 
          cbind(BorgOut[[f+1]][
            (ncol(BorgOut[[f+1]])-5):
              ncol(BorgOut[[f+1]])], f))
}
colnames(ObjectiveResultsToPlot) = ObjectiveNames
write.table(ObjectiveResultsToPlot, 
            "Objectives_bygroup.csv", sep = ",", 
            col.names = TRUE, row.names = FALSE)






