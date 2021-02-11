# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE D
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid); library(png)
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

Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)
satisficing_sols = which(as.numeric(as.character(Pareto$Rel)) <= 0.01 & 
                           as.numeric(as.character(Pareto$RF)) <= 0.2 & 
                           as.numeric(as.character(Pareto$WCC)) <= 0.1)
Satisficing = Pareto[satisficing_sols,]
Satisficing$Set = "Satisficing"; Satisficing$Solution = 1:nrow(Satisficing)

# read realization data for one solution from each agreement formulation
# (the +/-1 factor here is to adjust for R indexing starting at 1, but
#  C for WaterPaths indexes from 0)
f1_sim = 2702-1; f2_sim = 5065-1
f1_sim = 2401-1; f2_sim = 4873-1; f0_sim = 1054-1
f1_sim = 2132-1; f2_sim = 4873-1; f0_sim = 1054-1
f1_sim = 3613-1; f2_sim = 4249-1; f0_sim = 380-1
PTests = Pareto[c(f0_sim+1, f1_sim+1, f2_sim+1),] # confirm these are satisficing

### --------------------------------------------------------------------------------
# PLOT TO SHOW EXAMPLE SOLUTIONS ON TOP OF SATISFICING BACKGROUND
# make traditional parallel plot of results 
Objs = Pareto[,c(1:7,78,79)]
Objs$group = as.factor(Objs$group)
Objs$group = plyr::revalue(Objs$group, c('0'="3) No Agreement", '1'="1) Fixed Capacity", '2'="2) Adjustable Capacity"))
colnames(Objs) = c("Reliability", 
                   "Restriction\nUse", 
                   "Infrastructure\nNet Present Cost", 
                   "Peak Financial\nCost",
                   "Worst-Case\nCost",
                   "Unit Cost\nof Expansion", 
                   "Formulation", "Set", "Solution")
Objs$Satisficing = "Full reference set"
OS = Objs[which(Objs$Solution %in% satisficing_sols),]
OS$Satisficing = "Solutions meeting\ncriteria"
Objs = rbind(Objs, OS)

colmaxes = apply(Objs[,1:6],2,max); colmaxes = c(1,1,1000,3,4,4000)
colmins = apply(Objs[,1:6],2,min); colmins = c(0,0,0,0,0,0)
for (r in 1:nrow(Objs)) {
  Objs[r,1:6] = (Objs[r,1:6] - colmins) / (colmaxes - colmins)
}
OM = reshape2::melt(Objs, id = c("Formulation", "Solution", "Set", "Satisficing"))
OM_base = OM[which(OM$Satisficing != "Full reference set"),]
OM_base$Satisficing = "Solutions meeting\ncriteria"
f_colors = c(wes_palette(n = 3, name = "FantasticFox1"))

A_base = ggplot(data = OM)
for (base_set in unique(OM$Satisficing)) {
  A_base = A_base + 
    geom_line(data = OM[which(OM$Satisficing == base_set),], 
              aes(x = variable, y = value, group = Solution, color = Satisficing)) +
    scale_color_manual(values = c("grey90", "grey80")) + 
    theme(panel.background = element_rect(color = NA, fill = "transparent"),
          panel.grid.major.y = element_blank(),
          panel.grid.minor = element_blank(),
          legend.key = element_blank(),
          legend.background = element_rect(color = NA, fill = "transparent"),
          panel.grid.major.x = element_line(color = "black"), 
          plot.margin = unit(c(1,1,1,1), "in"),
          plot.background = element_rect(color = NA, fill = "transparent"),
          axis.text = element_blank(),
          axis.ticks.y = element_blank(), 
          axis.title = element_blank()) + 
    scale_y_continuous(expand = c(0,0), limits = c(0,1)) + labs(color = "Formulation")
}

# add text labels of objective bounds
MaxText = c("0%", "100%", "$1B", "300%", "400%", "$4/Gal")
MinText = c("100%", "0%", "$0", "0% AVR", "0% AVR", "$0/Gal")
ObjText = c("Reliability", "Use Restriction\nFrequency", "Infrastructure\nNet Present Cost", 
            "Peak Financial\nCost", "Worst-Case\nCost", "Unit Cost of\nExpansion")
xloc = c(0.5, 1, 1.5, 2, 2.5, 3) * 2
for (spot in 1:length(ObjText)) {
  A_base = A_base + 
    annotation_custom(grob = textGrob(MinText[spot], gp = gpar(fontsize = 8)),  
                      xmin = xloc[spot], xmax = xloc[spot], 
                      ymin = -0.06, ymax = -0.06) +
    annotation_custom(grob = textGrob(MaxText[spot], gp = gpar(fontsize = 8)),  
                      xmin = xloc[spot], xmax = xloc[spot], 
                      ymin = 1.06, ymax = 1.06) +
    annotation_custom(grob = textGrob(ObjText[spot], gp = gpar(fontsize = 7, fontface = "italic"), rot = 60),
                      xmin = xloc[spot]-0.2, xmax = xloc[spot]-0.2,
                      ymin = -0.35, ymax = -0.35)
}
A_base_table = ggplot_gtable(ggplot_build(A_base))
A_base_table$layout$clip[A_base_table$layout$name == "panel"] = "off"
ggsave(plot = A_base_table, "results_figures/D1_base.png", dpi = 1200, height = 3.5, width = 7, units = "in")

# add chosen solutions
f_colors = c(wes_palette(n = 3, name = "FantasticFox1"), "grey90", "grey80")
Chosen = OM_base[which(OM_base$Solution %in% c(f1_sim+1, f2_sim+1, f0_sim+1)),]
A_base_set = A_base + 
  geom_line(data = Chosen, aes(x = variable, y = value, group = Solution, colour = Formulation), size = 1.25) +
  scale_color_manual(values = f_colors) + 
  theme(panel.background = element_blank(),
        panel.grid.major.x = element_line(color = "black"), 
        plot.margin = unit(c(1,1,1,1), "in"),
        legend.key = element_blank(),
        axis.text = element_blank(),
        axis.ticks.y = element_blank(), 
        axis.title = element_blank()) + 
  scale_y_continuous(expand = c(0,0), limits = c(0,1)) + labs(color = "Formulation") +
  annotation_custom(grob = textGrob("A", gp = gpar(fontsize = 15, fontface = "bold", col = f_colors[1])),
                    xmin = 4.6, xmax = 4.6,
                    ymin = 0.35, ymax = 0.35) +
  annotation_custom(grob = textGrob("B", gp = gpar(fontsize = 15, fontface = "bold", col = f_colors[2])),
                    xmin = 5.7, xmax = 5.7,
                    ymin = 0.25, ymax = 0.25) +
  annotation_custom(grob = textGrob("C", gp = gpar(fontsize = 15, fontface = "bold", col = f_colors[3])),
                    xmin = 2.55, xmax = 2.55,
                    ymin = 0.6, ymax = 0.6)
A_base_set_table = ggplot_gtable(ggplot_build(A_base_set))
A_base_set_table$layout$clip[A_base_set_table$layout$name == "panel"] = "off"
ggsave(plot = A_base_set_table, paste("results_figures/D1.png", sep = ""), 
       dpi = 1200, height = 3.5, width = 7, units = "in", bg = "transparent")


### --------------------------------------------------------------------------------
# NOW PLOT REALIZATION-LEVEL DIFFERENCES IN WJLWTP ALLOCATIONS OVER TIME + FINANCING
#### Figure D: demonstrate WJLWTP allocations/finance differences with different demand realizations

# read bootstrap objective sets
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("No Agreement", "Fixed Allocations", "Adjustable Allocations")
# AllSols = c("Reliability", 
#             "Restriction\nUse", 
#             "Infrastructure\nNPC", 
#             "Peak Financial\nCost",
#             "Worst-Case\nMitigation Cost",
#             "Unit Cost\nof Expansion", 
#             "Utility", "Solution", "Formulation", "Case")
# case = c("Full Set", "High Demand", "Low Demand")
# for (s in Satisficing$Solution) {
#   BA = read.csv(paste("demand_bootstraps/objectives_all_reals_2_100_S", as.character(s-1), ".csv", sep = ""), header = FALSE)
#   BS = read.csv(paste("demand_bootstraps/bootstrap_objs_2_100_S", as.character(s-1), ".csv", sep = ""), header = FALSE)
#   BAll = list(BA, BS[1,], BS[2,])
#   c = 1
#   for (r in BAll) {
#     collect_objs = as.data.frame(matrix(as.numeric(as.character(r)), nrow = length(utilities), byrow = TRUE))
#     collect_objs$Utility = utilities; collect_objs$Solution = s
#     collect_objs$Formulation = formulations[Satisficing$group[s]+1]
#     collect_objs$Case = case[c] 
#     AllSols = rbind(AllSols, collect_objs)
#     c = c + 1
#   }
# }
# A = as.data.frame(AllSols[2:nrow(AllSols),]); colnames(A) = AllSols[1,]
# AM = reshape2::melt(A, id = c("Utility", "Solution", "Formulation", "Case"))
# DemandTestFig = ggplot(data = AM) + 
#   geom_boxplot(aes(x = Case, y = as.numeric(as.character(value)), 
#                    color = Formulation)) +
#   facet_wrap(variable ~ Utility, scales = "free_y") +
#   theme(axis.text.x = element_text(angle = 90))
# ggsave(paste("results_figures/DemandTestFig.png", sep = ""), 
#        dpi = 600, height = 15, width = 12, units = "in")

# pick one high and one low demand realization to show realization-level differences
select_realizations = read.csv("demand_bootstraps/old_bootstraps/bootstrap_realizations_2_100_S1.csv", header = FALSE)
high_reals = c(t(select_realizations[1,])) # also tested 142, 129, 126, 117, 116, 113
#low_real = select_realizations[2,100] # also tested 377, 376, 372, 371, 366, 363, 360, 355

#for (high_real in high_reals) { # for sim 2429, realizations 60, 236, 268 are good examples to pick
  high_real = 268
  low_real = 371
  
  # ------------------------------------------------------------------------------------------------------------------------
  # ------------------------------------------------------------------------------------------------------------------------
  # WHEN SIMULATIONS WERE RE-RUN FOR SATISFICING SOLUTIONS, THIS OVERWROTE THE FIRST 150-ISH REFERENCE SET SOLUTION PATHWAYS
  # SO BE AWARE OF THIS WHEN MAKING FIGURES AND WHEN PULLING DATA
  r_names = rownames(Satisficing)
  sol_ids = Satisficing$Solution-1
  actual_solution_ids_for_satisficing_set = sol_ids[which(r_names %in% as.character(c(f0_sim+1, f1_sim+1, f2_sim+1)))]
  
  # locate formulation 1 and 2 satisficing solutions to compare
  # based on objective value and WJLWTP DV similarities 
  SF1 = Satisficing[which(Satisficing$group == 1),c(1:6,42,43,59,60,79)]; SF1$SUM = rowSums(SF1[,7:10])
  SF2 = Satisficing[which(Satisficing$group == 2),c(1:6,42,43,59,60,79)]; SF2$SUM = rowSums(SF2[,7:10])
  
  # auto-generate Ubuntu command to pull realization files for specific formulations/simulations/realizations
  # this isn't the best way to do this, just gonna grab all the realizations for each sim over mobaxterm
  paste("scp spec689@thecube.cac.cornell.edu:~/AGUmodel2019/WaterPaths/output/", "0", "/sinusoidal/" , 
        "\\{WaterSources,Policies,Utilities\\}",
        "_s", as.character(f0_sim), 
        "_r\\{", as.character(low_real), ",", as.character(high_real), "\\}.csv",
        " /mnt/c/Users/dgorelic/'OneDrive - University of North Carolina at Chapel Hill'/UNC/Research/WSC/Coding/WP/BorgOutput/demand_bootstraps/", 
        sep = "")
  
  # ------------------------------------------------------------------------------------------------------------------------
  # ------------------------------------------------------------------------------------------------------------------------
  
  partner_utility_ids = c(0,1,4,5)
  r_set = c("High Demand", "Low Demand"); forms = c("1) Fixed Capacity", "2) Adjustable Capacity")
  wjlwtp_project_ids = c("X29trmt_alloc_0", "X29trmt_alloc_1", "X29trmt_alloc_4", "X29trmt_alloc_5",
                         "X30trmt_alloc_0", "X30trmt_alloc_1", "X30trmt_alloc_4", "X30trmt_alloc_5",
                         "X31trmt_alloc_0", "X31trmt_alloc_1", "X31trmt_alloc_4", "X31trmt_alloc_5",
                         "X32trmt_alloc_0", "X32trmt_alloc_1", "X32trmt_alloc_4", "X32trmt_alloc_5")
  utility_ds_cols = c("X0debt_serv", "X1debt_serv", "X4debt_serv", "X5debt_serv")
  AllInfo = c("WJLWTP Low - OWASA Allocation",
              "WJLWTP Low - Durham Allocation",
              "WJLWTP Low - Chatham Allocation",
              "WJLWTP Low - Pittsboro Allocation", 
              "WJLWTP High - OWASA Allocation",
              "WJLWTP High - Durham Allocation",
              "WJLWTP High - Chatham Allocation",
              "WJLWTP High - Pittsboro Allocation",
              # "OWASA Annual Debt Service", "Durham Annual Debt Service", 
              # "Chatham Annual Debt Service", "Pittsboro Annual Debt Service", 
              "Simulation", "Realization", "Formulation", "Week")
  utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
  formulations = c("No Agreement", "Fixed Allocations", "Adjustable Allocations")
  projects = c('Durham Reservoirs', "Falls Lake", "Wheeler Benson Lakes", 'Stone Quarry', # EXISTING
               'Cane Creek Reservoir', 'University Lake', 'Jordan Lake', 'Haw River Intake','Sanford Dummy Intake', # EXISTING
               'Litter River Reservoir - Raleigh', 'Richland Creek Quarry', # NEW RALEIGH
               'Teer Quarry Expansion', # NEW DURHAM
               'Neuse River Intake', 'Harnett County Intake', # NEW RALEIGH
               'Dummy Endpoint', # MODEL DUMMY ENDPOINT NODE
               'Stone Quarry Low Expansion','Stone Quarry High Expansion','University Lake Expansion', # NEW OWASA
               'Lake Michie Low Expansion','Lake Michie High Expansion', # NEW DURHAM
               'Falls Lake Reallocation', # NEW RALEIGH
               'Reclaimed Water Low Build', 'Reclaimed Water High Build', # NEW DURHAM
               'Cane Creek Reservoir Expansion', # NEW OWASA
               'Haw River Intake Low','Haw River Intake High', # NEW PITTSBORO
               'Cary WTP Upgrades', # NEW CARY
               'Sanford Intake Low','Sanford Intake High', # NEW PITTSBORO/CHATHAM
               'WJLWTP Low - Fixed','WJLWTP High - Fixed',
               'WJLWTP Low - Variable','WJLWTP High - Variable',
               'Cape Fear Intake') # NEW CARY
  Pathways = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
  id_set = list(c(1:8), c(9:16))
  f = 1
  for (s in actual_solution_ids_for_satisficing_set[2:3]) {
    i = 1
    
    # also get pathways for these simulations
    P = read.table(paste("demand_bootstraps/Pathways_s", as.character(s), ".out", sep = ""), 
                   sep = "\t", header = TRUE)
    P$Solution = s
    P$Formulation = formulations[Pareto$group[s]+1]
    
    Pathways = rbind(Pathways, P)
    
    for (r in c(high_real, low_real)) {
      WS = read.csv(paste("demand_bootstraps/WaterSources_s", 
                          as.character(s), "_r", 
                          as.character(r), ".csv", sep = ""), header = TRUE)
      # U  = read.csv(paste("demand_bootstraps/Utilities_s", 
      #                     as.character(s), "_r", 
      #                     as.character(r), ".csv", sep = ""), header = TRUE)
      
      collected_info = cbind(WS[,wjlwtp_project_ids[id_set[[f]]]])
      collected_info$Simulation = s; collected_info$Realization = r_set[i]
      collected_info$Formulation = forms[f]; collected_info$Week = 1:nrow(collected_info)
      colnames(collected_info) = 1:ncol(collected_info)
      AllInfo = rbind(AllInfo, collected_info)
      i = i + 1
    }
    f = f + 1
  }
  AllInfo = as.data.frame(AllInfo[2:nrow(AllInfo),])
  colnames(AllInfo) = c("WJLWTP Low Allocation - OWASA",
                        "WJLWTP Low Allocation - Durham",
                        "WJLWTP Low Allocation - Chatham",
                        "WJLWTP Low Allocation - Pittsboro", 
                        "WJLWTP High Allocation - OWASA",
                        "WJLWTP High Allocation - Durham",
                        "WJLWTP High Allocation - Chatham",
                        "WJLWTP High Allocation - Pittsboro",
                        "Simulation", "Realization", "Formulation", "Week")
  AMS = reshape2::melt(AllInfo, id = c("Simulation", "Realization", "Formulation", "Week"))
  AMS$Utility = sapply(strsplit(as.character(AMS$variable), " - "), "[[", 2)
  AMS$Variable = sapply(strsplit(as.character(AMS$variable), " - "), "[[", 1)
  AMS = AMS %>% select(-variable)
  Years = seq(26, by = 52, length.out = 45)
  AMSY = AMS[which(AMS$Week %in% Years),]
  
  AMSY$Week = as.numeric(as.character(AMSY$Week))
  AMSY$value = as.numeric(as.character(AMSY$value))
  
  # aggregate allocations across time, set years before project is built to zero
  # need to extract relative allocations and re-apply them to fixed infrastructure cost
  Pathways = Pathways[2:nrow(Pathways),]
  colnames(Pathways) = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
  Pathways$Utility = utilities[as.numeric(as.character(Pathways$Utility))+1]
  Pathways$Project = projects[as.numeric(as.character(Pathways$Project))+1]
  
  PW = Pathways[which(Pathways$Realization %in% c(high_real, low_real)),]
  WJLWTP_NoBuild = 2500
  
  WJLWTP_Low_HighDemand_BuildWeek_Fixed = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                     PW$Realization == high_real & 
                                                                     PW$Project == "WJLWTP Low - Fixed")])
  if (length(WJLWTP_Low_HighDemand_BuildWeek_Fixed) == 0) {WJLWTP_Low_HighDemand_BuildWeek_Fixed = WJLWTP_NoBuild}
  WJLWTP_High_HighDemand_BuildWeek_Fixed = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                      PW$Realization == high_real & 
                                                                      PW$Project == "WJLWTP High - Fixed")])
  if (length(WJLWTP_High_HighDemand_BuildWeek_Fixed) == 0) {WJLWTP_High_HighDemand_BuildWeek_Fixed = WJLWTP_NoBuild}
  WJLWTP_Low_LowDemand_BuildWeek_Fixed = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                    PW$Realization == low_real & 
                                                                    PW$Project == "WJLWTP Low - Fixed")])
  if (length(WJLWTP_Low_LowDemand_BuildWeek_Fixed) == 0) {WJLWTP_Low_LowDemand_BuildWeek_Fixed = WJLWTP_NoBuild}
  WJLWTP_High_LowDemand_BuildWeek_Fixed = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                     PW$Realization == low_real & 
                                                                     PW$Project == "WJLWTP High - Fixed")])
  if (length(WJLWTP_High_LowDemand_BuildWeek_Fixed) == 0) {WJLWTP_High_LowDemand_BuildWeek_Fixed = WJLWTP_NoBuild}
  
  WJLWTP_Low_HighDemand_BuildWeek_Variable = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                        PW$Realization == high_real & 
                                                                        PW$Project == "WJLWTP Low - Variable")])
  if (length(WJLWTP_Low_HighDemand_BuildWeek_Variable) == 0) {WJLWTP_Low_HighDemand_BuildWeek_Variable = WJLWTP_NoBuild}
  WJLWTP_High_HighDemand_BuildWeek_Variable = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                         PW$Realization == high_real & 
                                                                         PW$Project == "WJLWTP High - Variable")])
  if (length(WJLWTP_High_HighDemand_BuildWeek_Variable) == 0) {WJLWTP_High_HighDemand_BuildWeek_Variable = WJLWTP_NoBuild}
  WJLWTP_Low_LowDemand_BuildWeek_Variable = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                       PW$Realization == low_real & 
                                                                       PW$Project == "WJLWTP Low - Variable")])
  if (length(WJLWTP_Low_LowDemand_BuildWeek_Variable) == 0) {WJLWTP_Low_LowDemand_BuildWeek_Variable = WJLWTP_NoBuild}
  WJLWTP_High_LowDemand_BuildWeek_Variable = as.numeric(PW$Week[which(PW$Utility == "OWASA" & 
                                                                        PW$Realization == low_real & 
                                                                        PW$Project == "WJLWTP High - Variable")])
  if (length(WJLWTP_High_LowDemand_BuildWeek_Variable) == 0) {WJLWTP_High_LowDemand_BuildWeek_Variable = WJLWTP_NoBuild}
  
  WJLWTP_Low_LowDemand_BuildWeek_Fixed = min(WJLWTP_Low_LowDemand_BuildWeek_Fixed,
                                             WJLWTP_High_LowDemand_BuildWeek_Fixed)
  WJLWTP_Low_HighDemand_BuildWeek_Fixed = min(WJLWTP_Low_HighDemand_BuildWeek_Fixed,
                                              WJLWTP_High_HighDemand_BuildWeek_Fixed)
  WJLWTP_Low_LowDemand_BuildWeek_Variable = min(WJLWTP_Low_LowDemand_BuildWeek_Variable,
                                                WJLWTP_High_LowDemand_BuildWeek_Variable)
  WJLWTP_Low_HighDemand_BuildWeek_Variable = min(WJLWTP_Low_HighDemand_BuildWeek_Variable,
                                                 WJLWTP_High_HighDemand_BuildWeek_Variable)
  
  
  AMSY$value[which(AMSY$Week < WJLWTP_Low_HighDemand_BuildWeek_Fixed &
                     AMSY$Variable == "WJLWTP Low Allocation" &
                     AMSY$Formulation == "1) Fixed Capacity" &
                     AMSY$Realization == "High Demand")] = 0
  AMSY$value[which(AMSY$Week < WJLWTP_High_HighDemand_BuildWeek_Fixed &
                     AMSY$Variable == "WJLWTP High Allocation" &
                     AMSY$Formulation == "1) Fixed Capacity" &
                     AMSY$Realization == "High Demand")] = 0
  AMSY$value[which(AMSY$Week < WJLWTP_Low_LowDemand_BuildWeek_Fixed &
                     AMSY$Variable == "WJLWTP Low Allocation" &
                     AMSY$Formulation == "1) Fixed Capacity" &
                     AMSY$Realization == "Low Demand")] = 0
  AMSY$value[which(AMSY$Week < WJLWTP_High_LowDemand_BuildWeek_Fixed &
                     AMSY$Variable == "WJLWTP High Allocation" &
                     AMSY$Formulation == "1) Fixed Capacity" &
                     AMSY$Realization == "Low Demand")] = 0
  
  AMSY$value[which(AMSY$Week < WJLWTP_Low_HighDemand_BuildWeek_Variable &
                     AMSY$Variable == "WJLWTP Low Allocation" &
                     AMSY$Formulation == "2) Adjustable Capacity" &
                     AMSY$Realization == "High Demand")] = 0
  AMSY$value[which(AMSY$Week < WJLWTP_High_HighDemand_BuildWeek_Variable &
                     AMSY$Variable == "WJLWTP High Allocation" &
                     AMSY$Formulation == "2) Adjustable Capacity" &
                     AMSY$Realization == "High Demand")] = 0
  AMSY$value[which(AMSY$Week < WJLWTP_Low_LowDemand_BuildWeek_Variable &
                     AMSY$Variable == "WJLWTP Low Allocation" &
                     AMSY$Formulation == "2) Adjustable Capacity" &
                     AMSY$Realization == "Low Demand")] = 0
  AMSY$value[which(AMSY$Week < WJLWTP_NoBuild &
                     AMSY$Variable == "WJLWTP High Allocation" &
                     AMSY$Formulation == "2) Adjustable Capacity" &
                     AMSY$Realization == "Low Demand")] = 0
  
  # try summing allocations and plotting
  AMSYS = AMSY %>% group_by(Formulation, Realization, Utility, Week) %>% summarise(TotalAllocated = sum(value))
  f_colors = wes_palette(n = 6, name = "IsleofDogs1")[2:5]
  f_colors2 = c(wes_palette(n = 3, name = "FantasticFox1"), "grey80")
  ann_text = data.frame(lab = c("A", "B"), Week = rep(5*52,2), TotalAllocated = rep(30,2), ColorToBe = f_colors2[1:2],
                        Realization = rep("High Demand",2), Formulation = as.character(unique(AMSYS$Formulation)))
  D = ggplot(data = AMSYS) + 
    geom_bar(aes(x = (Week/52+2015), y = TotalAllocated/7, group = Utility, fill = Utility),
             color = NA, position = "stack", stat = "identity") +
    facet_grid(Realization ~ Formulation) +
    scale_fill_manual(values = f_colors) + xlab("Year") + ylab("Jordan Lake WTP Treatment Capacity Allocation (MGD)") +
    theme(strip.background = element_blank(), 
          strip.text.x = element_text(size = 15, face = "bold"),
          strip.text.y = element_text(size = 17),
          panel.background = element_rect(fill = "white", color = "black"),
          panel.grid.major = element_line(color = "grey90", size = 0.5),
          panel.grid.minor = element_line(color = "grey95", size = 0.25),
          legend.background = element_rect(fill = "transparent", color = NA),
          legend.key = element_rect(color = NA),
          plot.background = element_rect(color = NA, fill = "transparent"),
          panel.spacing = unit(1.3, "lines")) +
    scale_x_continuous(expand = c(0.1,0), limits = c(2015,2061)) + scale_y_continuous(expand = c(0,2)) +
    geom_text(data = ann_text, aes(x = 2015, y = 50, label = lab, colour = ColorToBe), size = 11, face = "bold") +
    scale_colour_manual(values = f_colors2[1:2]) + guides(colour = FALSE) +
    ggtitle("Comparison of cooperative formulations \nfor Jordan Lake WTP development")
  ggsave(paste("results_figures/D_r", as.character(high_real), ".png", sep = ""), 
         dpi = 1200, height = 5, width = 7, units = "in", bg = "transparent")
  
#}


# what are the DV treatment allocations for each utility in each sim?
colnames(PTests)[8:77] = DVnames
PTDVs = PTests[,c("Solution",
                  "WJLWTP Initial Treatment Allocation - OWASA", "WJLWTP Initial Treatment Allocation - Durham",
                  "WJLWTP Initial Treatment Allocation - Chatham", "WJLWTP Initial Treatment Allocation - Pittsboro")]

# estimate financing shares 
WJLWTP_Low_Capital_Cost_AnnualDS = 16.23 # million
WJLWTP_High_Capital_Cost_AnnualDS = 20.1 # assuming low NOT already built, true for these two realizations/sims

# with the current realizations we are using, the high demand ones build the high WJLWTP first
# and the low demand ones never build, so this simplifies the debt service plotting
# in Sim 2131, Real 366, OWASA builds the low WJLWTP for $2.97M in debt service annually for 25 years, has a 0.183 allocation fraction
#   meaning annual debt service on low WJLWTP is about $16.23M annually
# in Real 116, OWASA builds only high WJLWTP for $3.677M annually, works out to $20.1M annually for whole project
# include relative shares of total debt service paid by each utility in pie chart inset plots on figure D
AMSYSD = AMSYS[which(AMSYS$TotalAllocated > 0),]
AMSYSDF = AMSYSD %>% group_by(Formulation, Realization, Week) %>% mutate(AnnualTotalAllocated = TotalAllocated/sum(TotalAllocated))
AMSYSDF$DebtService = NA
AMSYSDF$DebtService[which(AMSYSDF$Realization == "High Demand")] = 
  AMSYSDF$AnnualTotalAllocated[which(AMSYSDF$Realization == "High Demand")] * WJLWTP_High_Capital_Cost_AnnualDS
AMSYSDF$DebtService[which(AMSYSDF$Realization == "Low Demand")] = 
  AMSYSDF$AnnualTotalAllocated[which(AMSYSDF$Realization == "Low Demand")] * WJLWTP_Low_Capital_Cost_AnnualDS

# sum over 25 year debt repayment period, ignoring years past the maturity date
AMSYSDF$DebtService[which(AMSYSDF$Week > (WJLWTP_High_HighDemand_BuildWeek_Fixed+52*25) & AMSYSDF$Realization == "High Demand")] = 0
AMSYSDF$DebtService[which(AMSYSDF$Week > (WJLWTP_Low_HighDemand_BuildWeek_Fixed+52*25) & AMSYSDF$Realization == "Low Demand")] = 0
AMSYSDF$DebtService[which(AMSYSDF$Week > (WJLWTP_Low_LowDemand_BuildWeek_Fixed+52*25) & 
                            AMSYSDF$Realization == "Low Demand" & AMSYSDF$Formulation == "1) Fixed Capacity")] = 0
AMSYSDF$DebtService[which(AMSYSDF$Week > (WJLWTP_Low_LowDemand_BuildWeek_Variable+52*25) & 
                            AMSYSDF$Realization == "Low Demand" & AMSYSDF$Formulation == "2) Adjustable Capacity")] = 0


annotation_custom2 <- 
  function (grob, xmin = -Inf, xmax = Inf, ymin = -Inf, ymax = Inf, data){ layer(data = data, stat = StatIdentity, position = PositionIdentity, 
                                                                                 geom = ggplot2:::GeomCustomAnn,
                                                                                 inherit.aes = TRUE, params = list(grob = grob, 
                                                                                                                   xmin = xmin, xmax = xmax, 
                                                                                                                   ymin = ymin, ymax = ymax))}
DSsums = AMSYSDF %>% group_by(Formulation, Realization, Utility) %>% summarize(Total = sum(DebtService))
D_set = D
for (s in unique(DSsums$Formulation)) {
  for (r in unique(DSsums$Realization)) {
    DSSR = DSsums[which(DSsums$Formulation == s & DSsums$Realization == r),]
    D_pie = ggplot(data = DSSR) + 
      geom_bar(aes(x = "", fill = Utility, y = Total), 
               color = "white", stat = "identity", width = 1) + 
      coord_polar("y", start = 0) +
      theme_void() + theme(legend.position = "none", 
                           panel.background = element_rect(fill = "transparent"),
                           plot.background = element_rect(fill = "transparent", color = NA)) + 
      scale_fill_manual(values = f_colors)
    ggsave(plot = D_pie, paste("results_figures/D_pie_", s, "_", r, ".png", sep = ""), 
           dpi = 1200, units = "in", height = 2, width = 2, bg = "transparent")
    
    # add pie charts to D with annotation for final figure
    pie_img = readPNG(paste("results_figures/D_pie_", s, "_", r, ".png", sep = ""))
    D_set = D_set + annotation_custom2(rasterGrob(pie_img, interpolate = TRUE),
                                       xmin = 2010, xmax = 2026, ymin = -3, ymax = 24, data = DSSR)
    if (s == "1) Fixed Capacity" & r == "High Demand") {
      D_set = D_set + annotation_custom2(grob = textGrob("Total Debt\nService\nShares", gp = gpar(fontsize = 7)),  
                                         xmin = 2018, xmax = 2018, 
                                         ymin = 26.2, ymax = 26.2, data = DSSR)
    }
  }
}
D_final = ggplot_gtable(ggplot_build(D_set))
D_final$layout$clip[D_final$layout$name == "panel"] = "off"
ggsave(plot = D_final, "results_figures/D.png", 
       dpi = 1200, units = "in", height = 5, width = 7, bg = "transparent")





