# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE B
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)
satisficing_sols = which(as.numeric(as.character(Pareto$Rel)) <= 0.01 & 
                           as.numeric(as.character(Pareto$RF)) <= 0.2 & 
                           as.numeric(as.character(Pareto$WCC)) <= 0.1)

# after running Satisficing_DVs.csv with WaterPaths (using flag -p false) to get utility objective values
# collect them here from each formulation's set for additional plotting (sim_objectives_run.sh on Cube)
# ADJUSTED TO COLLECT ALL PARETO REFERENCE SET UTILITY OBJECTIVES

# Oct 2020 reference set: sols 0-1093 are F0
#                         sols 1094-3900 are F1
#                         sols 3901-4828 are F2 (remember R index "Solutions" starting at 1, not 0)
# NOTE: because i got the index wrong on these by one for cube runs, will be manually 
#       adjusting files to add sol 1094 to F1 file and sol 3901 to F2 file
f0_sols = "reeval_sim_objectives/Objectives_sols0_1094.csv"
f1_sols = "reeval_sim_objectives/Objectives_sols1094_3901.csv"
f2_sols = "reeval_sim_objectives/Objectives_sols3901_4829.csv"
files_to_read = c(f0_sols, f1_sols, f2_sols)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("No Agreement", "Fixed Allocations", "Adjustible Allocations"); f = 1
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
                          c(utilities[which.min(collect_objs[,1])], 
                            utilities[apply(collect_objs[,2:6], MARGIN = 2, which.max)], 
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

# export organized utility-level objective results
#write.table(AllSols, "reeval_sim_objectives/ParetoObjectives_byUtility.csv", sep = ",", row.names = FALSE)
#write.table(MinimaxDriver, "reeval_sim_objectives/ParetoObjectives_byUtility_MinimaxDriver.csv", sep = ",", row.names = FALSE)

# bar chart showing frequency of Utility Driving Minimax outcomes
MinimaxDriver$Satisficing = "Full Reference Set" 
MDS = MinimaxDriver[which(MinimaxDriver$Solution %in% satisficing_sols),]
MDS$Satisficing = "Solutions Meeting Criteria"
MinimaxDriver = rbind(MinimaxDriver, MDS)

MinimaxDriver$Formulation = plyr::revalue(as.factor(MinimaxDriver$Formulation), 
                                          c('Fixed Allocations'="1) Fixed Capacity", 
                                            'Adjustible Allocations'="2) Adjustable Capacity", 
                                            'No Agreement'="3) No Agreement"))
MinimaxDriver$Formulation = factor(MinimaxDriver$Formulation, levels = c("1) Fixed Capacity", 
                                                                         "2) Adjustable Capacity", 
                                                                         "3) No Agreement"))

f_colors = wes_palette(n = 6, name = "IsleofDogs1")
MM = reshape2::melt(MinimaxDriver, id = c("Solution", "Formulation", "Satisficing"))
Ac = ggplot(data = MM) + 
  geom_bar(aes(x = variable, fill = value), 
           color = NA, position = position_fill()) + 
  facet_grid(Satisficing ~ Formulation, scales = "free_y", switch = "y") + 
  theme(axis.text.x = element_text(angle = 90, vjust = 0.33), 
        plot.background = element_rect(fill = "transparent", color = NA),
        legend.title = element_blank(), 
        legend.background = element_rect(fill = "transparent", color = NA),
        legend.key = element_rect(color = NA),
        strip.background = element_rect(fill = "transparent", color = NA),
        strip.text.y = element_text(face = "bold", color = "black", size = 9),
        strip.text.x = element_text(color = "black", size = 14),
        strip.placement = "outside",
        panel.grid = element_blank(),
        axis.ticks = element_blank(),
        panel.background = element_rect(fill = "transparent", color = NA)) + 
  scale_fill_manual(values = f_colors) + scale_y_continuous(breaks = c(0,1), labels = c("0%", "100%")) +
  ylab("Proportion of Solutions") + xlab("Objective") + 
  ggtitle("Utility responsible for regional objective values:\nfull reference set vs. solutions meeting performance criteria")
ggsave(paste("results_figures/B", ".png", sep = ""), 
       dpi = 1200, units = "in", width = 8.2, height = 5.4, bg = "transparent")



