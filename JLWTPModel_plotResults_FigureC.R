# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE C
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
formulations = c("3) No Agreement", "1) Fixed Capacity", "2) Adjustable Capacity"); f = 1
AllSols = c("Reliability", 
            "Restriction\nUse", 
            "Infrastructure\nNet Present Cost", 
            "Peak Financial\nCost",
            "Worst-Case\nn Cost",
            "Unit Cost\nof Expansion", 
            "Utility", "Solution", "Formulation")

allsims = 1
for (set in files_to_read) {
  S = read.csv(set, header = FALSE)
  for (sim in 1:nrow(S)) {
    collect_objs = as.data.frame(matrix(as.numeric(as.character(S[sim,])), nrow = length(utilities), byrow = TRUE))
    collect_objs$Utility = utilities; collect_objs$Solution = allsims; collect_objs$Formulation = formulations[f]
    collect_objs$Satisficing = allsims %in% satisficing_sols
    AllSols = rbind(AllSols, collect_objs)

    allsims = allsims + 1
  }
  f = f + 1
}
AllSols = as.data.frame(AllSols[2:nrow(AllSols),]); colnames(AllSols) = c("Reliability", 
                                                                          "Restriction\nUse", 
                                                                          "Infrastructure\nNet Present Cost", 
                                                                          "Peak Financial\nCost",
                                                                          "Worst-Case\n Cost",
                                                                          "Unit Cost\nof Expansion", 
                                                                          "Utility", "Solution", "Formulation",
                                                                          "Satisficing")

# export for use elsewhere
write.table(AllSols,'reeval_sim_objectives/Objectives_byutility.csv', sep = ",", row.names = FALSE)

##### -------------------------------------------------------------------------------------------
#### FIGURE AA: look at objective results across WJLWTP partners
# breakdown by utility
# run to check spread of solutions across formulations: summary(as.factor(Satisficing$group))
ASS = AllSols[which(AllSols$Solution %in% satisficing_sols),]
AllSols$Satisficing = "Full Reference Set"; ASS$Satisficing = "Satisficing Solutions"
AllSols = rbind(AllSols, ASS)
ASS$Satisficing = "Satisficing Solutions"
AllSols = rbind(AllSols, ASS)

partner_utilities = c("Chatham", "Durham", "OWASA", "Pittsboro")
key_objectives = c("Infrastructure\nNet Present Cost", "Peak Financial\nCost")
ASP = AllSols[which(AllSols$Utility %in% partner_utilities),]
ASPM = reshape2::melt(ASP, id = c("Utility", "Solution", "Formulation", "Satisficing"))
ASPMO = ASPM[which(ASPM$variable %in% key_objectives),]

ASPMO_base = ASPMO[which(ASPMO$Satisficing == "Full Reference Set"),]
ASPMO_top = ASPMO[which(ASPMO$Satisficing == "Satisficing Solutions"),]
# Aa_base = ggplot(data = ASPMO_base) + 
#   geom_line(aes(x = Utility, y = as.numeric(value), group = Solution, color = Satisficing),
#             size = 1.5) + scale_color_manual(values = c("grey80")) +
#   facet_grid(variable ~ Formulation, scales = "free_y") + 
#   theme(panel.background = element_rect(fill = "white"), 
#         panel.grid.major.x = element_line(color = "grey10", size = 1),
#         panel.spacing.y = unit(1, "lines"), panel.spacing.x = unit(3, "lines"),
#         strip.background = element_blank(), strip.text = element_text(size = 20, face = "bold"),
#         legend.position = "none") +
#   xlab("Western Jordan Lake Joint WTP Partner Utility") + ylab("Objective Value") +
#   scale_x_discrete(expand = c(0,0)) + scale_y_continuous(expand = c(0,0))
# ggsave(plot = Aa_base, "results_figures/Aa_base.png", dpi = 600, height = 8, width = 14, units = "in")

# f_colors = c(wes_palette(n = 3, name = "FantasticFox1")[c(2,1,3)], "grey80")
# Aa_full = ggplot(data = ASPMO_top) + 
#   geom_line(aes(x = Utility, y = as.numeric(value), group = Solution, color = Formulation),
#             size = 0.7) + scale_color_manual(values = f_colors) +
#   theme(panel.background = element_rect(fill = "white"), 
#         axis.text.x = element_text(angle = 90, size = 12), 
#         axis.title = element_text(size = 15),
#         panel.grid.major.x = element_line(color = "grey10", size = 1),
#         strip.background = element_blank(), strip.text = element_text(size = 20, face = "bold"),
#         legend.position = "none") +
#   facet_grid(variable ~ Formulation, scales = "free_y") +
#   xlab("Western Jordan Lake Joint WTP Partner Utility") + ylab("Objective Value") +
#   scale_x_discrete(expand = c(0.01,0.01)) + scale_y_continuous(expand = c(0,0))
# ggsave(plot = Aa_full, "results_figures/Aa_all.png", dpi = 600, height = 8, width = 11.5, units = "in")

# version with boxplots
f_colors = c(wes_palette(n = 3, name = "FantasticFox1"), "grey80")
# ASPMO_top$Formulation = plyr::revalue(as.factor(ASPMO_top$Formulation),
#                                       c('Fixed Allocations'="1) Fixed Capacity",
#                                         'Adjustible Allocations'="2) Adjustible Capacity",
#                                         'No Agreement'="3) No Agreement"))
ASPMO_top$Formulation = factor(ASPMO_top$Formulation, levels = c("1) Fixed Capacity", 
                                                                 "2) Adjustable Capacity", 
                                                                 "3) No Agreement"))
ASPMO_top$variable = plyr::revalue(as.factor(ASPMO_top$variable), 
                                   c('Infrastructure\nNet Present Cost'="Infrastructure Net Present Cost\n($ millions, present-valued)", 
                                     'Peak Financial\nCost'="Peak Financial Cost\n(fraction of AVR)"))
Aa_full = ggplot(data = ASPMO_top) + 
  geom_boxplot(aes(x = Utility, y = as.numeric(value), group = paste(Utility,Formulation), color = Formulation),
               size = 0.7) + scale_color_manual(values = f_colors) +
  theme(panel.background = element_rect(fill = "white", color = "black", size = 2), 
        axis.text.x = element_text(size = 12), 
        axis.title.x = element_text(size = 15),
        strip.placement = "outside",
        legend.key = element_rect(fill = "transparent", color = NA, size = 3),
        legend.background = element_rect(color = "black", fill = "grey90"),
        legend.position = c(0.97,0.97), legend.justification = c(1,1),
        axis.title.y = element_blank(),
        plot.background = element_rect(color = NA, fill = "transparent"),
        panel.grid.major.x = element_blank(),
        panel.grid.major.y = element_line(color = "grey90", size = 0.7),
        panel.grid.minor = element_blank(),
        axis.ticks.x = element_blank(),
        panel.spacing.y = unit(1, "lines"), panel.spacing.x = unit(2, "lines"),
        strip.background = element_blank(), strip.text = element_text(size = 10, color = "black")) +
  facet_grid(variable ~ ., scales = "free_y", switch = "y") +
  xlab("Jordan Lake Partnering Utilities") + labs(color = "Formulation") +
  scale_x_discrete(expand = c(0.12,0.12)) + scale_y_continuous(expand = c(0.02,0.05))
ggsave(plot = Aa_full, "results_figures/C.png", 
       dpi = 1200, height = 5, width = 4.5, units = "in", bg = "transparent")
