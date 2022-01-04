# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Jun 2021)
#   REEVALUATE SATISFICING SOLUTIONS UNDER NEW SOWS
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid); library(png)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

##### ---------------------------------------------------------------------------------------
####  Read in re-evaluated satisficing solutions to see if they still meet criteria
#RS0 = read.csv("reeval_sim_objectives/reeval_test/Objectives_sols0_to_30.csv", header = FALSE)
#RS1 = read.csv("reeval_sim_objectives/reeval_test/Objectives_sols30_to_536.csv", header = FALSE)
#RS2 = read.csv("reeval_sim_objectives/reeval_test/Objectives_sols536_to_588.csv", header = FALSE)

f0_sols = "reeval_sim_objectives/reeval_test/Objectives_sols0_to_30.csv"
f1_sols = "reeval_sim_objectives/reeval_test/Objectives_sols30_to_536.csv"
f2_sols = "reeval_sim_objectives/reeval_test/Objectives_sols536_to_588.csv"
files_to_read = c(f0_sols, f1_sols, f2_sols)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("No Agreement", "Fixed Capacity", "Adjustable Capacity"); f = 1
MinimaxDriver = c("Reliability", 
                  "Restriction\nUse", 
                  "Infrastructure\nNet Present Cost", 
                  "Peak Financial\nCost",
                  "Worst-Case\nCost",
                  "Unit Cost\nof Expansion", 
                  "Solution", "Formulation")
allsims = 1
for (set in files_to_read) {
  S = read.csv(set, header = FALSE)
  for (sim in 1:nrow(S)) {
    collect_objs = as.data.frame(matrix(as.numeric(as.character(S[sim,])), nrow = length(utilities), byrow = TRUE))
    collect_objs$Utility = utilities; collect_objs$Solution = allsims; collect_objs$Formulation = formulations[f]
    
    MinimaxDriver = rbind(MinimaxDriver,
                          c(min(collect_objs[,1]), 
                            apply(collect_objs[,2:6], MARGIN = 2, max), 
                            allsims, formulations[f]))
    allsims = allsims + 1
  }
  f = f + 1
}
MinimaxDriver = as.data.frame(MinimaxDriver[2:nrow(MinimaxDriver),]); colnames(MinimaxDriver) = c("Reliability", 
                                                                                                  "Restriction\nUse", 
                                                                                                  "Infrastructure\nNet Present Cost", 
                                                                                                  "Peak Financial\nCost",
                                                                                                  "Worst-Case\nCost",
                                                                                                  "Unit Cost\nof Expansion", 
                                                                                                  "Solution", "Formulation")



# get the real set for comparison
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_mixedNFE.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)
satisficing_sols = which(as.numeric(as.character(Pareto$Rel)) <= 0.01 & 
                           as.numeric(as.character(Pareto$RF)) <= 0.2 & 
                           as.numeric(as.character(Pareto$WCC)) <= 0.05)
Satisficing = Pareto[satisficing_sols,]
IDs = Satisficing$Solution
summary(as.factor(Satisficing$group))
Satisficing$Set = "Satisficing"; Satisficing$Solution = 1:nrow(Satisficing)

Objs = Satisficing[,c(1:6,79, 7)]
Objs$group = as.factor(Objs$group)
Objs$group = plyr::revalue(Objs$group, c('0'="No Agreement", 
                                         '1'="Fixed Capacity", 
                                         '2'="Adjustable Capacity"))
colnames(Objs) = c("Reliability", 
                   "Restriction\nUse", 
                   "Infrastructure\nNet Present Cost", 
                   "Peak Financial\nCost",
                   "Worst-Case\nCost",
                   "Unit Cost\nof Expansion", 
                   "Solution", "Formulation")

Objs$Set = "Base"
MinimaxDriver$Set = "Re-evaluation"

# which solutions are the WCC outliers?
# ones previously chosen as examples in paper f1_sim = 16797-1; f2_sim = 29091-1; f0_sim = 698-1
#MinimaxDriver$RFsols = IDs

# grab statistics to report about number of sols in each set that meet criteria
keep_sols = which(as.numeric(as.character(MinimaxDriver$Reliability)) >= 0.986 &
                           as.numeric(as.character(MinimaxDriver$`Restriction
Use`)) <= 0.2 &
                           as.numeric(as.character(MinimaxDriver$`Worst-Case
Cost`)) <= 0.1)

summary(as.factor(Satisficing$group)) # base set
30/588; 506/588; 52/588
summary(as.factor(MinimaxDriver$Formulation[keep_sols]))
19/434; 385/434; 30/434 # with 0.99 rel
25/501; 435/501; 41/501 # with 0.988 rel
26/546; 473/546; 47/546 # with 0.986 rel



# combine for plotting
library(reshape2)
OM = melt(Objs[keep_sols,], id = c("Solution", "Formulation", "Set"))
MM = melt(MinimaxDriver[keep_sols,], id = c("Solution", "Formulation", "Set"))
OM$`Base Objective Value` = OM$value; OM$`Re-evaluation Objective Value` = as.numeric(as.character(MM$value))
OM$`Base Objective Value`[which(OM$variable == "Reliability")] = 1 - OM$`Base Objective Value`[which(OM$variable == "Reliability")]

library(ggplot2); library(wesanderson)
i = 1
mini = c(0.97,0,0,0,0,0)
maxi = c(1,0.25,1000,2,0.2,4000)
for (obj in unique(OM$variable)) {
  supfig = ggplot(data = OM[which(OM$variable == obj),]) + 
    geom_point(aes(x = `Base Objective Value`, y = `Re-evaluation Objective Value`,
                   color = Formulation, group = Solution)) +
    facet_wrap(Formulation ~ variable, ncol = 1) +
    guides(color = FALSE) + scale_color_manual(values = wes_palette(n = 3, name = "FantasticFox1")[c(3,1,2)]) +
    xlim(c(mini[i], maxi[i])) + ylim(c(mini[i], maxi[i]))
  ggsave(paste("results_figures/ReevaluationComparisonTest_", i, ".png", sep = ""), dpi = 400, units = "in", width = 2, height = 8)
  i = i + 1
}





