# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
DVnames = c('ROF Trigger - Durham Restrictions', 
            'ROF Trigger - OWASA Restrictions', 
            'ROF Trigger - Raleigh Restrictions', 
            'ROF Trigger - Cary Restrictions', 
            'ROF Trigger - Durham Transfers',
            'ROF Trigger - OWASA Transfers',
            'ROF Trigger - Raleigh Transfers', 
            'JL Allocation - OWASA', 
            'JL Allocation - Durham', 
            'JL Allocation - Cary', 
            'Reserve Fund Contribution - Durham', 
            'Reserve Fund Contribution - OWASA', 
            'Reserve Fund Contribution - Raleigh', 
            'Reserve Fund Contribution - Cary', 
            'ROF Trigger - Durham Infrastructure', 
            'ROF Trigger - OWASA Infrastructure', 
            'ROF Trigger - Raleigh Infrastructure',  
            'Infrastructure Rank - University Lake Expansion', 
            'Infrastructure Rank - Cane Creek Reservoir Expansion', 
            'Infrastructure Rank - Stone Quarry Low Expansion', 
            'Infrastructure Rank - Stone Quarry High Expansion',
            'Infrastructure Rank - Teer Quarry Expansion',
            'Infrastructure Rank - Reclaimed Water Low Build', 
            'Infrastructure Rank - Reclaimed Water High Build',
            'Infrastructure Rank - Lake Michie Low Expansion',
            'Infrastructure Rank - Lake Michie High Expansion', 
            'Infrastructure Rank - Little River Reservoir Build',
            'Infrastructure Rank - Richland Creek Quarry Build',
            'Infrastructure Rank - Neuse River Intake Build', 
            'Infrastructure Rank - Falls Lake Reallocation', 
            'Infrastructure Rank - WJLWTP Low OWASA',
            'Infrastructure Rank - WJLWTP High OWASA',
            'Infrastructure Rank - WJLWTP Low Durham',
            'Infrastructure Rank - WJLWTP High Durham',
            'WJLWTP Initial Treatment Allocation - OWASA', 
            'WJLWTP Initial Treatment Allocation - Durham', 
            'Demand Buffer - Durham', 
            'Demand Buffer - OWASA', 
            'Demand Buffer - Raleigh', 
            'ROF Trigger - Pittsboro Restrictions', 
            'ROF Trigger - Pittsboro Transfers', 
            'JL Allocation - Pittsboro', 
            'Reserve Fund Contribution - Pittsboro', 
            'ROF Trigger - Pittsboro Infrastructure', 
            'Demand Buffer - Pittsboro',
            'ROF Trigger - Chatham Restrictions', 
            'ROF Trigger - Chatham Transfers', 
            'JL Allocation - Chatham', 
            'Reserve Fund Contribution - Chatham', 
            'ROF Trigger - Chatham Infrastructure', 
            'Demand Buffer - Chatham',
            'WJLWTP Initial Treatment Allocation - Pittsboro', 
            'WJLWTP Initial Treatment Allocation - Chatham', 
            'Infrastructure Rank - WJLWTP Low Pittsboro',
            'Infrastructure Rank - WJLWTP High Pittsboro',
            'Infrastructure Rank - WJLWTP Low Chatham',
            'Infrastructure Rank - WJLWTP High Chatham', 
            'Infrastructure Rank - Sanford Intake Low Pittsboro',
            'Infrastructure Rank - Sanford Intake High Pittsboro',
            'Infrastructure Rank - Sanford Intake Low Chatham',
            'Infrastructure Rank - Sanford Intake High Chatham',
            'Infrastructure Rank - Haw River Intake Low Pittsboro',
            'Infrastructure Rank - Haw River Intake High Pittsboro',
            'Infrastructure Rank - Cape Fear Intake High Cary', 
            'Demand Projection - Forecast Length', 
            'Demand Projection - Historical Range', 
            'Demand Projection - Frequency',
            'ROF Trigger - Cary Infrastructure', 
            'Demand Buffer - Cary', 
            'Infrastructure Rank - Sanford WTP Expansion Cary')

# read in necessary objective and dv data
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_mixedNFE.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)
satisficing_sols = which(as.numeric(as.character(Pareto$Rel)) <= 0.01 & 
                           as.numeric(as.character(Pareto$RF)) <= 0.2 & 
                           as.numeric(as.character(Pareto$WCC)) <= 0.05)

# after running Satisficing_DVs.csv with WaterPaths (using flag -p false) to get utility objective values
# collect them here from each formulation's set for additional plotting (sim_objectives_run.sh on Cube)
# ADJUSTED TO COLLECT ALL PARETO REFERENCE SET UTILITY OBJECTIVES

# Oct 2020 reference set: sols 0-1093 are F0
#                         sols 1094-3900 are F1
#                         sols 3901-4828 are F2 (remember R index "Solutions" starting at 1, not 0)
# NOTE: because i got the index wrong on these by one for cube runs, will be manually 
#       adjusting files to add sol 1094 to F1 file and sol 3901 to F2 file
f0_sols = "reeval_sim_objectives/Objectives_sols0_to_3082.csv"
f1_sols = "reeval_sim_objectives/Objectives_sols3082_to_28597.csv"
f2_sols = "reeval_sim_objectives/Objectives_sols28597_to_29654.csv"
files_to_read = c(f0_sols, f1_sols, f2_sols)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("3) No Agreement", "1) Fixed Capacity", "2) Adjustable Capacity"); f = 1
AllSols = c("Reliability", 
            "Restriction\nUse", 
            "Infrastructure\nNPC", 
            "Peak Financial\nCost",
            "Worst-Case\n Cost",
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

# save time and write this to read back later?
write.csv(AllSols, "AllSols.csv", row.names = FALSE, col.names = TRUE)

# start here with the data
AllSols = read.csv("AllSols.csv", header = TRUE)
colnames(AllSols) = c("Reliability", 
                      "Restriction\nUse", 
                      "Infrastructure\nNet Present Cost", 
                      "Peak Financial\nCost",
                      "Worst-Case\n Cost",
                      "Unit Cost\nof Expansion", 
                      "Utility", "Solution", "Formulation",
                      "Satisficing")

# tile plot of correlations between utility-level objective values and DVs
Pareto$Formulation = formulations[as.numeric(as.character(Pareto$group))+1]
DVs = Pareto[,c(1:6,8:77,79,80)]
DVs$Formulation = Pareto$group; DVs$Solution = Pareto$Solution
DVs$Formulation = plyr::revalue(as.factor(DVs$Formulation), 
                                          c('1'="1) Fixed Capacity", 
                                            '2'="2) Adjustable Capacity", 
                                            '0'="3) No Agreement"))
TP = merge(AllSols, DVs, by = c("Formulation","Solution"))
colnames(TP)[17:86] = DVnames

TPS = TP[which(TP$Solution %in% satisficing_sols),]
formulations = c("1) Fixed Capacity", "2) Adjustable Capacity")
wjlwtp_utilities = c("OWASA", "Durham", "Chatham", "Pittsboro")
TPF = TPS[which(TPS$Formulation %in% formulations), c(1,3:9,17:20,56,62,21:23,57,63,24:26,58,64,
                                                      31:33,84,60,66,51,52,68,69)]
TPF = TPS[which(TPS$Formulation %in% formulations), c(1,3:9,51,52,68,69)] # just WJLWTP
TPFall = c(); library(Hmisc)
for (f in formulations) {
  for (u in wjlwtp_utilities) {
    # calculate correlations for each formulation-utility subset and rejoin
    TPFU = TPF[which(TPF$Formulation == f & TPF$Utility == u),-c(1,8)]
    CM = cor(data.matrix(TPFU), method = "spearman")
    colnames(CM) = colnames(TPFU); rownames(CM) = colnames(TPFU)
    
    # are correlations significant?
    for (row in 1:nrow(TPFU))
    CMs_Corrs = Hmisc::rcorr(data.matrix(TPFU), type = "spearman")
    CMs = CMs_Corrs$P
    colnames(CMs) = colnames(TPFU); rownames(CMs) = colnames(TPFU)
    
    # if working with just WJLWTP DVs, reduce col names
    if (nrow(CM) == 10) {
      colnames(CM)[7:10] = c("OWASA", "Durham", "Pittsboro", "Chatham")
      rownames(CM)[7:10] = c("OWASA", "Durham", "Pittsboro", "Chatham")
      
      colnames(CMs)[7:10] = c("OWASA", "Durham", "Pittsboro", "Chatham")
      rownames(CMs)[7:10] = c("OWASA", "Durham", "Pittsboro", "Chatham")
    }
    CMm = na.omit(reshape2::melt(CM)); CMm$Utility = u; CMm$Formulation = f; CMm$Type = "Coefficient"
    CMsm = na.omit(reshape2::melt(CMs)); CMsm$Utility = u; CMsm$Formulation = f; CMsm$Type = "Significance"
    TPFall = rbind(TPFall, CMm, CMsm)
  }
}

financial_objectives = c("Infrastructure\nNet Present Cost", "Peak Financial\nCost", "Unit Cost\nof Expansion")
TPFF = TPFall[which(TPFall$Var1 %in% financial_objectives & TPFall$Var2 %in% colnames(CM)[7:ncol(TPFU)]),]

# replace significance levels with asterisks for plotting, by thresholds
TPFF_sign = TPFF[which(TPFF$Type == "Significance"),]
TPFF_sign$Word = TPFF_sign$value
TPFF_sign$Word[which(TPFF_sign$Word <= 0.05)] = "*"
TPFF_sign$Word[which(TPFF_sign$value <= 0.01)] = "**"
TPFF_sign$Word[which(TPFF_sign$value <= 0.001)] = "***"
TPFF_sign$Word[which(TPFF_sign$Word > 0.05)] = NA

TPFF_corr = TPFF[which(TPFF$Type != "Significance"),]
tempplot = ggplot(data = TPFF_corr) + 
  geom_tile(aes(x = Var1, y = Var2, fill = value)) + 
  facet_grid(Formulation ~ Utility, switch = "x") +
  scale_fill_gradient2(high = "green4", low = "purple4", mid = "white", limits = c(-1,1)) + 
#  ggtitle(paste("Correlations between Objectives and Decision Variables", sep = "")) +
  theme(axis.text.x = element_text(angle = 90, vjust = 0.33), 
        axis.text = element_text(size = 11),
        axis.ticks = element_blank(), 
        panel.background = element_blank(),
        legend.position = "right", 
        legend.direction = "vertical", 
        strip.text = element_text(face = "bold", size = 13),
        strip.placement.x = "outside",
        legend.background = element_rect(color = "black", size = 1, fill = "grey90")) + 
  ylab("Jordan Lake WTP Initial Treatment Allocation\n(Decision Variable - Fraction of Available Capacity)") +
  xlab("Individual Utility Objective") +
  guides(fill = guide_legend(title = "Correlation\nCoefficient"))

tempplot + geom_text(data = TPFF_sign, aes(x = Var1, y = Var2, label = Word))
ggsave(paste("results_figures/H/H_Satisficing_Absolutes_significance.png", sep = ""), 
       dpi = 1200, units = "in", width = 8, height = 6)

# scatterplots for sanity check, by utility and formulation
TPFS = TPF[which(TPF$Utility %in% wjlwtp_utilities),]
TPFS$Solution = rep(1:558, each = 4)
TPSFOm = reshape2::melt(TPFS[,c(1:8,13)], id = c("Formulation", "Utility", "Solution"))
TPSFDm = reshape2::melt(TPFS[,c(1,8:13)], id = c("Formulation", "Utility", "Solution"))
TPSFall = merge(TPSFOm, TPSFDm, by = c("Formulation", "Utility", "Solution"))
for (f in formulations) {
  for (u in wjlwtp_utilities) {
    TPSFS = TPSFall[which(TPSFall$Formulation == f & TPSFall$Utility == u & TPSFall$variable.x %in% financial_objectives),]
    tempplot = ggplot(data = TPSFS) + 
      geom_point(aes(x = as.numeric(as.character(value.x)), 
                     y = as.numeric(as.character(value.y)))) + facet_grid(variable.y ~ variable.x, scales = "free") +
      scale_fill_gradient2(high = "blue2", low = "red2", mid = "white", limits = c(-1,1)) + 
      ggtitle(paste("Relationship between ", u, " Objectives and Decision Variables: \n", 
                    f, " Formulation", sep = "")) +
      theme(axis.text.x = element_text(angle = 90), 
            axis.ticks = element_blank(), 
            #            panel.background = element_blank(),
            legend.position = "right", 
            legend.direction = "vertical", 
            legend.background = element_rect(color = "black", size = 1, fill = "grey90")) + 
      ylab("Decision Variable Value") + xlab("Utility Objective Value")
    ggsave(paste("results_figures/H/H_", f, "_", u, "_Satisficing_scatter.png", sep = ""), 
           dpi = 400, units = "in", width = 9, height = 13)
    
  }
}
