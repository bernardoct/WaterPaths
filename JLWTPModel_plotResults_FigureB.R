# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE B
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)
satisficing_sols = which(Pareto$Rel <= 0.01 & Pareto$RF <= 0.2 & Pareto$WCC <= 0.1)

# after running Satisficing_DVs.csv with WaterPaths (using flag -p false) to get utility objective values
# collect them here from each formulation's set for additional plotting (sim_objectives_run.sh on Cube)
# ADJUSTED TO COLLECT ALL PARETO REFERENCE SET UTILITY OBJECTIVES
f0_sols = "reeval_sim_objectives/Objectives_sols0_to_1364.csv"
f1_sols = "reeval_sim_objectives/Objectives_sols1364_to_4418.csv"
f2_sols = "reeval_sim_objectives/Objectives_sols4418_to_5324.csv"
files_to_read = c(f0_sols, f1_sols, f2_sols)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("No Agreement", "Fixed Allocations", "Adjustible Allocations"); f = 1
MinimaxDriver = c("Reliability", 
                  "Restriction\nUse", 
                  "Infrastructure\nNPC", 
                  "Peak Financial\nCost",
                  "Worst-Case\nMitigation Cost",
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
                                                                                                  "Infrastructure\nNPC", 
                                                                                                  "Peak Financial\nCost",
                                                                                                  "Worst-Case\nMitigation Cost",
                                                                                                  "Unit Cost\nof Expansion", 
                                                                                                  "Solution", "Formulation")

# export organized utility-level objective results
#write.table(AllSols, "reeval_sim_objectives/ParetoObjectives_byUtility.csv", sep = ",", row.names = FALSE)
#write.table(MinimaxDriver, "reeval_sim_objectives/ParetoObjectives_byUtility_MinimaxDriver.csv", sep = ",", row.names = FALSE)

# bar chart showing frequency of Utility Driving Minimax outcomes
MinimaxDriver$Satisficing = "Full Reference Set" 
MDS = MinimaxDriver[which(MinimaxDriver$Solution %in% satisficing_sols),]
MDS$Satisficing = "Satisficing Solutions"
MinimaxDriver = rbind(MinimaxDriver, MDS)

MinimaxDriver$Formulation = plyr::revalue(as.factor(MinimaxDriver$Formulation), 
                                          c('Fixed Allocations'="1) Fixed Capacity", 
                                            'Adjustible Allocations'="2) Adjustible Capacity", 
                                            'No Agreement'="3) No Agreement"))
MinimaxDriver$Formulation = factor(MinimaxDriver$Formulation, levels = c("1) Fixed Capacity", 
                                                                         "2) Adjustible Capacity", 
                                                                         "3) No Agreement"))

f_colors = wes_palette(n = 6, name = "IsleofDogs1")
MM = reshape2::melt(MinimaxDriver, id = c("Solution", "Formulation", "Satisficing"))
Ac = ggplot(data = MM) + 
  geom_bar(aes(x = variable, fill = value), 
           color = NA, position = position_fill()) + 
  facet_grid(Satisficing ~ Formulation, scales = "free_y", switch = "y") + 
  theme(axis.text.x = element_text(angle = 90, vjust = 0.33), 
        legend.title = element_blank(), 
        axis.text.y = element_blank(), axis.title.y = element_blank(),
        strip.background = element_rect(fill = "white", color = "white"),
        strip.text = element_text(face = "bold", color = "black", size = 10),
        axis.ticks = element_blank(),
        panel.background = element_blank()) + 
  scale_fill_manual(values = f_colors) + scale_y_continuous(breaks = c(0,1)) +
  ylab("Fraction of Solutions") + xlab("Objective") + 
  ggtitle("Utility driving minimax regional objective values across each objective:\nfull reference set vs. satisficing solutions only")
ggsave(paste("results_figures/B", ".png", sep = ""), dpi = 400, units = "in", width = 8, height = 5)



