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

##### ---------------------------------------------------------------------------------------
#### Figure D: Look at probabilistic pathways from all reference set solutions
####  NOTE: SOME OF THE LARGER FILES HERE ONLY NEED TO BE MADE ONCE
####        SKIP DOWN TO LATER LINES TO READ AND RUN THOSE FILES
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("No Agreement", "Fixed Allocations", "Adjustible Allocations")
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
for (p in 1:nrow(Pareto)) {
  P = read.table(paste("sim_pathways/Pathways_s", as.character(p-1), ".out", sep = ""), 
                 sep = "\t", header = TRUE)
  P$Solution = p-1
  P$Formulation = formulations[Pareto$group[p]+1]
  
  Pathways = rbind(Pathways, P)
  
  # when sets get large, output to partial file
  if (p %% 200 == 0) {
    Pathways = Pathways[2:nrow(Pathways),]
    colnames(Pathways) = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
    
    Pathways$Utility = utilities[as.numeric(as.character(Pathways$Utility))+1]
    Pathways$Project = projects[as.numeric(as.character(Pathways$Project))+1]
    
    print(paste("Writing sim_pathways/Pathways_Full_set_", as.character(p), ".csv", sep = ""))
    write.table(Pathways, paste("sim_pathways/Pathways_Full_set_", as.character(p), ".csv", sep = ""), 
                sep = ",", row.names = FALSE, col.names = TRUE)
    Pathways = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
  }
}
# write last set
Pathways = Pathways[2:nrow(Pathways),]
colnames(Pathways) = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")

Pathways$Utility = utilities[as.numeric(as.character(Pathways$Utility))+1]
Pathways$Project = projects[as.numeric(as.character(Pathways$Project))+1]

print(paste("Writing sim_pathways/Pathways_Full_set_", as.character(p), ".csv", sep = ""))
write.table(Pathways, paste("sim_pathways/Pathways_Full_set_", as.character(p), ".csv", sep = ""), 
            sep = ",", row.names = FALSE, col.names = TRUE)

# combine to full set...
fs = list.files("sim_pathways", pattern = "Pathways_Full_*")
Pathways = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
for (f in fs) {
  print(paste("Reading ", f, sep = ""))
  P = read.table(paste("sim_pathways/", f, sep = ""), 
                       sep = ",", header = TRUE)
  Pathways = rbind(Pathways, P)
}
PW = Pathways[2:nrow(Pathways),]
write.table(PW, paste("sim_pathways/Pathways_Full.csv", sep = ""), 
            sep = ",", row.names = FALSE, col.names = TRUE)

# for initial plotting, just use satisficing set. full set is too large
satisficing_sols = which(Pareto$Rel <= 0.01 & Pareto$RF <= 0.2 & Pareto$WCC <= 0.1)
satisficing_sols = satisficing_sols - 1
PWS = PW[which(PW$Solution %in% satisficing_sols),]
write.table(PWS, paste("sim_pathways/Pathways_Full_Satisficing.csv", sep = ""), 
            sep = ",", row.names = FALSE, col.names = TRUE)

#### READ BACK IN FILES HERE THAT WERE GENERATED ABOVE, SAVE TIME
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
PWS = read.csv("sim_pathways/Pathways_Full_Satisficing.csv", header = TRUE)

# get sinusoidal factors for all realizations to make some subset plots of pathways
# with different demand levels
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/RealizationGeneration")
sinusoidal_multiplier = function(time, A, Tau, p) {return(1 + A * sin(2 * pi * time / Tau + p) - A * sin(p))}

# read sinusoidal parameter set and get demands of last 10 years
LHS_parameter_samples = as.matrix(read.table("RDMfactor_sets/demand_factors.csv", sep = ",", header = FALSE))
weeks = 1:2496; n_reals = 500
multipliers = matrix(NA, nrow = n_reals, ncol = length(weeks))
for (r in 1:n_reals) {
  for (w in weeks) {
    multipliers[r,(w-weeks[1]+1)] = as.numeric(as.character(sinusoidal_multiplier(time = w, 
                                                                                  A = LHS_parameter_samples[r,1], 
                                                                                  Tau = LHS_parameter_samples[r,2], 
                                                                                  p = LHS_parameter_samples[r,3])))
  }
}
# find realizations with greatest/lowest average values across 5 year periods
periods = c(52*10, 52*20, 52*30, 52*40, ncol(multipliers)); real_id = 0:(nrow(multipliers)-1)
sets = c("Realization", "Period", "Demand")
for (period in periods) {
  real_avg = as.data.frame(apply(multipliers[,(period-52*10+1):period], MARGIN = 1, mean)); real_avg$ID = 1:nrow(multipliers)
  real_sort = real_avg[order(real_avg$`apply(multipliers[, (period - 52 * 10 + 1):period], MARGIN = 1, mean)`, decreasing = TRUE),]
  real_tops = data.frame(Realization = real_sort[1:100,2]-1); real_tops$Period = period; real_tops$Demand = "High"
  real_lows = data.frame(Realization = real_sort[401:500,2]-1); real_lows$Period = period; real_lows$Demand = "Low"
  sets = rbind(sets, real_tops, real_lows)
}
sets = as.data.frame(sets[2:nrow(sets),])

# plot subsets of groupings
library(ggplot2)
PWS$Project = as.character(PWS$Project)
PWS$Project[which(PWS$Project %in% c('WJLWTP Low - Fixed','WJLWTP Low - Variable'))] = "WJLWTP Low"
PWS$Project[which(PWS$Project %in% c('WJLWTP High - Fixed','WJLWTP High - Variable'))] = "WJLWTP High"
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
#for (p in unique(sets$Period)) {
  p = sets$Period[length(sets$Period)] # just do final period for now
  
  P = sets[which(sets$Period == p & sets$Demand == "High"),]
  PWSH = PWS[which(PWS$Realization %in% P$Realization),]; PWSH$Demand = "High Demand"
  
  P = sets[which(sets$Period == p & sets$Demand == "Low"),]
  PWSL = PWS[which(PWS$Realization %in% P$Realization),]; PWSL$Demand = "Low Demand"
  
  PWSS = rbind(PWSH, PWSL)
  for (u in utilities) {
    PWSU = PWSS[which(PWSS$Utility == u),]
    if (u == "Raleigh") {PWSU = PWSU[which(!(PWSU$Project %in% c("WJLWTP High", "WJLWTP Low"))),]}
    
    D = ggplot(data = PWSU) +
      geom_histogram(aes(x = as.numeric(as.character(Week)), y = ..count.., fill = Project), 
                     binwidth = 52, position = "stack") +
      ggtitle(paste(u, "pathways in High/Low demand realizations")) +
      facet_grid(Formulation ~ Demand, scales = "free_y") + xlab("Week")
    ggsave(paste("results_figures/D_", u, "_", p, ".png", sep = ""), 
           dpi = 600, height = 10, width = 8, units = "in")
  }
#}

# try to construct plot showing each individual realizations
library(dplyr)
for (u in utilities) {
  PWSU = PWSS[which(PWSS$Utility == u),]
  if (u == "Raleigh") {PWSU = PWSU[which(!(PWSU$Project %in% c("WJLWTP High", "WJLWTP Low"))),]}
  
  # can we sort the realizations first by total infra built?
  count_per_realization = PWSU %>% group_by(Utility, Formulation, Solution, Demand, Realization) %>% 
    summarize(n_Projects = n()) %>% arrange(n_Projects, .by_group = TRUE) %>% mutate(Rank = row_number(n_Projects))
  PWSUC = merge(PWSU, count_per_realization, by = c("Utility", "Formulation", "Solution", "Demand", "Realization"))
  
  D = ggplot(data = PWSUC) +
    geom_tile(aes(y = as.numeric(as.character(Week))/52, x = as.factor(Rank+Solution), 
                  color = Project, fill = Project), 
              position = "identity", stat = "identity", alpha = 0.2, size = 0.5) + coord_flip() +
    ggtitle(paste(u, "pathways in High/Low demand realizations")) + 
    theme(axis.text.y = element_blank(), axis.title.y = element_blank(), axis.ticks.y = element_blank()) +
    facet_wrap(Demand ~ Formulation, nrow = 2, scales = "free_y") + ylab("Year")
  ggsave(paste("results_figures/D_", u, "_", p, "_RealizationPathways.png", sep = ""), 
         dpi = 600, height = 8, width = 12, units = "in")
}



###### ---------------------------------------------------------------------------------------------------------
###### ---------------------------------------------------------------------------------------------------------
###### ---------------------------------------------------------------------------------------------------------
###### EXPLORATORY PLOTTING CODE BELOW (UNUSED FOR FINAL FIGURES)

### THIS CODE FROM FIGURE B SCRIPT  
# tile plot of correlations between utility-level objective values and DVs
Pareto$Formulation = MinimaxDriver$Formulation
TP = merge(AllSols, Pareto[,c(7:77,79,80)], by = c("Formulation","Solution"))
colnames(TP)[11:80] = DVnames; colnames(TP)[10] = "Formulation"
colnames(TP)[3:8] = c("Reliability", 
                      "Restriction Use", 
                      "Infrastructure NPC", 
                      "Peak Financial Cost",
                      "Worst-Case Mitigation Cost",
                      "Unit Cost of Expansion")

for (u in utilities) {
  TPU = TP[which(TP$Utility == u), c(2:8,10:80)]
  CM = cor(data.matrix(TPU), use = "complete.obs")
  colnames(CM) = colnames(TPU)
  CM = CM[9:78,2:8]
  
  # only keep values for strong connections
  #  CM[abs(CM) < 0.2] = NA
  
  CMm = na.omit(reshape2::melt(CM))
  tempplot = ggplot(data = CMm) + 
    geom_tile(aes(x = Var1, y = Var2, fill = value)) + 
    scale_fill_gradient2(high = "blue2", low = "red2", mid = "white", limits = c(-1,1)) + 
    ggtitle(paste("Correlations between ", u, " Objectives and Decision Variables", sep = "")) +
    theme(axis.text.x = element_text(angle = 90), 
          axis.ticks = element_blank(), 
          panel.background = element_blank(),
          legend.position = "right", 
          legend.direction = "vertical", 
          legend.background = element_rect(color = "black", size = 1, fill = "grey90")) + 
    ylab("Objective") + xlab("Decision Variable")
  ggsave(paste("results_figures/Ab_", u, ".png", sep = ""), dpi = 400, units = "in", width = 17, height = 6)
}

# repeat for satisficing sols only
TPS = TP[which(TP$Solution %in% satisficing_sols),]
for (u in utilities) {
  TPU = TPS[which(TPS$Utility == u), c(2:8,10:80)]
  CM = cor(data.matrix(TPU), use = "complete.obs")
  colnames(CM) = colnames(TPU)
  CM = CM[9:78,2:8]
  
  # only keep values for strong connections
  #  CM[abs(CM) < 0.2] = NA
  
  CMm = na.omit(reshape2::melt(CM))
  tempplot = ggplot(data = CMm) + 
    geom_tile(aes(x = Var1, y = Var2, fill = value)) + 
    scale_fill_gradient2(high = "blue2", low = "red2", mid = "white", limits = c(-1,1)) + 
    ggtitle(paste("Correlations between ", u, " Objectives and Decision Variables", sep = "")) +
    theme(axis.text.x = element_text(angle = 90), 
          axis.ticks = element_blank(), 
          panel.background = element_blank(),
          legend.position = "right", 
          legend.direction = "vertical", 
          legend.background = element_rect(color = "black", size = 1, fill = "grey90")) + 
    ylab("Objective") + xlab("Decision Variable")
  ggsave(paste("results_figures/Ab_", u, "_Satisficing.png", sep = ""), dpi = 400, units = "in", width = 17, height = 6)
}




##### ---------------------------------------------------------------------------------------
#### Figure B: facets of single objectives vs. key DVs, colored by minimax utility

# organize DVs by utility, collect Infrastructure Rank DVs separately for now
DVs = Pareto[,8:77]
colnames(DVs) = DVnames

# reorganize data for plotting
DVs$Formulation = plyr::revalue(as.factor(Pareto$group), 
                                c('0'="No Agreement", '1'="Fixed Capacity Agreement", '2'="Adjustible Capacity Agreement"))
DVs$Satisficing = FALSE; DVs$Satisficing[satisficing_sols] = TRUE
DVs$Solution = 1:nrow(DVs)
DVM = reshape2::melt(DVs, id = c("Formulation", "Satisficing", "Solution"))

# deconstruct DV names to organize
DVM$Type = NA; DVM$Detail = NA; DVM$Utility = NA

# manually add utility names for some infrastructure rank DVs
overwrite_cases = colnames(DVs)[c(18:30,65:67)]
overwrite_value = c("OWASA", "OWASA", "OWASA", "OWASA", 
                    "Durham", "Durham", "Durham", "Durham", "Durham", 
                    "Raleigh", "Raleigh", "Raleigh", "Raleigh",
                    NA, NA, NA)

DVM$Type = sapply(strsplit(as.character(DVM$variable), " - "), "[[", 1)
DVM$Detail = sapply(strsplit(as.character(DVM$variable), " - "), "[[", 2)
for (u in utilities) {
  util_check = sapply(sapply(strsplit(DVM$Detail, " "), function(x) {x}), function(x) {u %in% x})
  DVM$Utility[which(util_check == TRUE)] = u
}
for (case in overwrite_cases) {
  DVM$Utility[which(DVM$variable == case)] = overwrite_value[which(overwrite_cases == case)]
}

# remove utility names from detail column
for (u in utilities) {
  util_check = sapply(sapply(strsplit(DVM$Detail, " "), function(x) {x}), function(x) {u %in% x})
  name_without_utility = sapply(sapply(strsplit(DVM$Detail[which(util_check == TRUE)], " "), function(x) {x}), 
                                function(x) {paste(x[grep(u, x, invert = TRUE)], collapse = " ")})
  
  DVM$Detail[which(util_check == TRUE)] = name_without_utility
}

# write dataset if we want it later
#write.table(DVM, "melted_Pareto_DVs.csv", sep = ",", row.names = FALSE, col.names = TRUE)

#DVM_satisficing = DVM[which(DVM$Satisficing == TRUE),]
fig_sets = c("a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k"); f = 1
for (dv_set in unique(DVM$Type)) {
  if (dv_set == "Infrastructure Rank") {
    g = 1
    for (u in utilities) {
      # sort infrastructure order to see relative build order
      #      for (s in unique(DVM$Solution[which(DVM$Type == dv_set & DVM$Utility == u)])) {
      #        DVM$value[which(DVM$Type == dv_set & DVM$Utility == u & DVM$Solution == s)] = 
      #          order(DVM$value[which(DVM$Type == dv_set & DVM$Utility == u & DVM$Solution == s)])
      #      }
      B = ggplot(data = DVM[which(DVM$Type == dv_set & DVM$Utility == u),]) + 
        geom_violin(aes(x = Detail, y = value, color = Formulation, fill = Formulation), 
                    scale = "width", 
                    position = position_dodge(1)) +
        facet_wrap(Satisficing ~ ., scales = "free_y") +
        theme(axis.text.x = element_text(angle = 90)) +
        ggtitle(paste(dv_set, u, sep = " - "))
      ggsave(paste("results_figures/B", fig_sets[f], as.character(g), ".png", sep = ""), 
             dpi = 600, height = 5, width = 12, units = "in")
      g = g + 1
    }
  } else {
    B = ggplot(data = DVM[which(DVM$Type == dv_set),]) + 
      geom_violin(aes(x = Utility, y = value, color = Formulation, fill = Formulation), 
                  scale = "width", position = position_dodge(1)) +
      facet_wrap(Satisficing ~ Detail, scales = "free_y") +
      theme(axis.text.x = element_text(angle = 90)) +
      ggtitle(dv_set)
    ggsave(paste("results_figures/B", fig_sets[f], ".png", sep = ""), 
           dpi = 600, height = 5, width = 12, units = "in")
  }
  f = f + 1
}




##### --------------------------------------------------------------------------------
#### old other code

for (f in formulations) {
  for (u in utilities) {
    TPU = TP[which(TP$Utility == u & TP$Formulation == f), c(3:8,11:86)]
    CM = cor(data.matrix(TPU), method = "spearman")
    colnames(CM) = colnames(TPU)
    #    CM = CM[9:78,2:8]
    
    # only keep values for strong connections
    #  CM[abs(CM) < 0.2] = NA
    
    CMm = na.omit(reshape2::melt(CM))
    tempplot = ggplot(data = CMm) + 
      geom_tile(aes(x = Var1, y = Var2, fill = value)) + 
      scale_fill_gradient2(high = "blue2", low = "red2", mid = "white", limits = c(-1,1)) + 
      ggtitle(paste("Correlations between ", u, " Objectives and Decision Variables under a ", 
                    f, " Formulation", sep = "")) +
      theme(axis.text.x = element_text(angle = 90), 
            axis.ticks = element_blank(), 
            panel.background = element_blank(),
            legend.position = "right", 
            legend.direction = "vertical", 
            legend.background = element_rect(color = "black", size = 1, fill = "grey90")) + 
      ylab("Objective") + xlab("Decision Variable")
    ggsave(paste("results_figures/H/H_", f, "_", u, ".png", sep = ""), dpi = 400, units = "in", width = 17, height = 17)
  }
}


# repeat for satisficing sols only
TPS = TP[which(TP$Solution %in% satisficing_sols),]
for (f in formulations) {
  for (u in utilities) {
    TPU = TPS[which(TPS$Utility == u & TPS$Formulation == f), c(3:8,11:86)]
    CM = cor(data.matrix(TPU), method = "spearman")
    colnames(CM) = colnames(TPU)
    #  CM = CM[9:78,2:8]
    
    # only keep values for strong connections
    #  CM[abs(CM) < 0.2] = NA
    
    CMm = na.omit(reshape2::melt(CM))
    tempplot = ggplot(data = CMm) + 
      geom_tile(aes(x = Var1, y = Var2, fill = value)) + 
      scale_fill_gradient2(high = "blue2", low = "red2", mid = "white", limits = c(-1,1)) + 
      ggtitle(paste("Correlations between ", u, " Objectives \nand Decision Variables under a \n", 
                    f, " Formulation", sep = "")) +
      theme(axis.text.x = element_text(angle = 90), 
            axis.ticks = element_blank(), 
            panel.background = element_blank(),
            legend.position = "right", 
            legend.direction = "vertical", 
            legend.background = element_rect(color = "black", size = 1, fill = "grey90")) + 
      ylab("Objective") + xlab("Decision Variable")
    ggsave(paste("results_figures/H/H_", f, "_", u, "_Satisficing.png", sep = ""), dpi = 400, units = "in", width = 17, height = 17)
  }
}

##### ---------------------------------------------------------------------------------------
#### collect data and then drop down here to make new figures
#### Dec 2020: comparing WJLWTP initial allocations by formulation to indiv. utility financial objs

# set subset of data to compare
cols_to_keep = c("Formulation","Solution","Satisficing","Utility",
                 "Reliability", "Restriction\nUse",
                 "Infrastructure\nNPC", "Peak Financial\nCost",
                 "Worst-Case\n Cost", "Unit Cost\nof Expansion",
                 "WJLWTP Initial Treatment Allocation - OWASA", "WJLWTP Initial Treatment Allocation - Durham",
                 "WJLWTP Initial Treatment Allocation - Pittsboro", "WJLWTP Initial Treatment Allocation - Chatham")
TPS = TP[,cols_to_keep]
TPSm = reshape2::melt(TPS, id = c("Formulation","Solution","Satisficing","Utility",
                                  "Reliability", "Restriction\nUse",
                                  "Infrastructure\nNPC", "Peak Financial\nCost",
                                  "Worst-Case\n Cost", "Unit Cost\nof Expansion"))
TPSm$DV = sapply(strsplit(as.character(TPSm$variable), " - "), "[[", 1)
TPSm$`WJLWTP Utility` = sapply(strsplit(as.character(TPSm$variable), " - "), "[[", 2)

TPSmerge = TPS[,c("Formulation","Solution","Satisficing","Utility","WJLWTP Initial Treatment Allocation - Chatham")]
colnames(TPSmerge)[5] = "Chatham Allocation"

TPSA = merge(TPSm, TPSmerge, by = c("Formulation","Solution","Satisficing","Utility"))

# select financial objectives for JL utilities
wjlwtp_utilities = c("Chatham", "Durham", "OWASA", "Pittsboro")
objs_to_keep = c("Formulation","Solution","Satisficing","Utility", "WJLWTP Utility", "DV",
                 "Infrastructure\nNPC", "Peak Financial\nCost", "Worst-Case\n Cost",
                 "Chatham Allocation", "value")
TPSO = TPSA[which(TPSA$Utility %in% wjlwtp_utilities & TPSA$Formulation != "3) No Agreement"), objs_to_keep]

# show financial objectives vs. chatham allocation for each utility by formulation
# goal: show chathams influence on other utilities' objectives with adjustable agreement
H = ggplot(data = TPSO) + 
  geom_bin2d(aes(y = as.numeric(as.character(`Infrastructure\nNPC`)), 
                 x = as.numeric(as.character(`Chatham Allocation`)),
                 fill = ..density..), bins = 10) +
  facet_grid(Utility ~ Formulation, scales = "free_y") +
  guides(color = FALSE, fill = FALSE) +
  theme(axis.text.x = element_text(angle = 90))
ggsave(paste("results_figures/H", "_test.png", sep = ""), 
       dpi = 600, height = 8, width = 6, units = "in")


##### ---------------------------------------------------------------------------------------
#### Figure B: facets of single objectives vs. key DVs, colored by minimax utility

# organize DVs by utility, collect Infrastructure Rank DVs separately for now
DVs = Pareto[,8:77]
colnames(DVs) = DVnames

# reorganize data for plotting
DVs$Formulation = plyr::revalue(as.factor(Pareto$group), 
                                c('0'="No Agreement", '1'="Fixed Capacity", '2'="Adjustable Capacity"))
DVs$Satisficing = FALSE; DVs$Satisficing[satisficing_sols] = TRUE
DVs$Solution = 1:nrow(DVs)
DVM = reshape2::melt(DVs, id = c("Formulation", "Satisficing", "Solution"))

# deconstruct DV names to organize
DVM$Type = NA; DVM$Detail = NA; DVM$Utility = NA

# manually add utility names for some infrastructure rank DVs
overwrite_cases = colnames(DVs)[c(18:30,65:67)]
overwrite_value = c("OWASA", "OWASA", "OWASA", "OWASA", 
                    "Durham", "Durham", "Durham", "Durham", "Durham", 
                    "Raleigh", "Raleigh", "Raleigh", "Raleigh",
                    NA, NA, NA)

DVM$Type = sapply(strsplit(as.character(DVM$variable), " - "), "[[", 1)
DVM$Detail = sapply(strsplit(as.character(DVM$variable), " - "), "[[", 2)
for (u in utilities) {
  util_check = sapply(sapply(strsplit(DVM$Detail, " "), function(x) {x}), function(x) {u %in% x})
  DVM$Utility[which(util_check == TRUE)] = u
}
for (case in overwrite_cases) {
  DVM$Utility[which(DVM$variable == case)] = overwrite_value[which(overwrite_cases == case)]
}

# remove utility names from detail column
for (u in utilities) {
  util_check = sapply(sapply(strsplit(DVM$Detail, " "), function(x) {x}), function(x) {u %in% x})
  name_without_utility = sapply(sapply(strsplit(DVM$Detail[which(util_check == TRUE)], " "), function(x) {x}), 
                                function(x) {paste(x[grep(u, x, invert = TRUE)], collapse = " ")})
  
  DVM$Detail[which(util_check == TRUE)] = name_without_utility
}

# write dataset if we want it later
#write.table(DVM, "melted_Pareto_DVs.csv", sep = ",", row.names = FALSE, col.names = TRUE)

#DVM_satisficing = DVM[which(DVM$Satisficing == TRUE),]
fig_sets = c("a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k"); f = 1
for (dv_set in unique(DVM$Type)) {
  if (dv_set == "Infrastructure Rank") {
    g = 1
    for (u in utilities) {
      # sort infrastructure order to see relative build order
      #      for (s in unique(DVM$Solution[which(DVM$Type == dv_set & DVM$Utility == u)])) {
      #        DVM$value[which(DVM$Type == dv_set & DVM$Utility == u & DVM$Solution == s)] = 
      #          order(DVM$value[which(DVM$Type == dv_set & DVM$Utility == u & DVM$Solution == s)])
      #      }
      B = ggplot(data = DVM[which(DVM$Type == dv_set & DVM$Utility == u),]) + 
        geom_violin(aes(x = Detail, y = value, color = Formulation, fill = Formulation), 
                    scale = "width", 
                    position = position_dodge(1)) +
        facet_wrap(Satisficing ~ ., scales = "free_y") +
        theme(axis.text.x = element_text(angle = 90)) +
        ggtitle(paste(dv_set, u, sep = " - "))
      ggsave(paste("results_figures/B", fig_sets[f], as.character(g), ".png", sep = ""), 
             dpi = 600, height = 5, width = 12, units = "in")
      g = g + 1
    }
  } else {
    B = ggplot(data = DVM[which(DVM$Type == dv_set),]) + 
      geom_violin(aes(x = Utility, y = value, color = Formulation, fill = Formulation), 
                  scale = "width", position = position_dodge(1)) +
      facet_wrap(Satisficing ~ Detail, scales = "free_y") +
      theme(axis.text.x = element_text(angle = 90)) +
      ggtitle(dv_set)
    ggsave(paste("results_figures/B", fig_sets[f], ".png", sep = ""), 
           dpi = 600, height = 5, width = 12, units = "in")
  }
  f = f + 1
}





