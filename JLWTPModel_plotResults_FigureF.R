# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE F
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
annotation_custom2 <- function (grob, xmin = -Inf, xmax = Inf, ymin = -Inf, ymax = Inf, data) { 
  layer(data = data, stat = StatIdentity, position = PositionIdentity, 
        geom = ggplot2:::GeomCustomAnn,inherit.aes = TRUE, params = list(grob = grob, xmin = xmin, xmax = xmax, ymin = ymin, ymax = ymax))}

# necessary info 
f1_sim = 2132-1; f2_sim = 4873-1; f0_sim = 1054-1
f1_sim = 3613-1; f2_sim = 4249-1; f0_sim = 380-1

Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)
satisficing_sols = which(as.numeric(as.character(Pareto$Rel)) <= 0.01 & 
                           as.numeric(as.character(Pareto$RF)) <= 0.2 & 
                           as.numeric(as.character(Pareto$WCC)) <= 0.1)
Satisficing = Pareto[satisficing_sols,]
Satisficing$Set = "Satisficing"; Satisficing$Solution = 1:nrow(Satisficing)

r_names = rownames(Satisficing)
sol_ids = Satisficing$Solution-1
actual_solution_ids_for_satisficing_set = sol_ids[which(r_names %in% as.character(c(f0_sim+1, f1_sim+1, f2_sim+1)))]
f1_sim = actual_solution_ids_for_satisficing_set[2]
f2_sim = actual_solution_ids_for_satisficing_set[3] 
f0_sim = actual_solution_ids_for_satisficing_set[1]

high_real = 116; low_real = 366
high_real = 268; low_real = 371
f = 1; r_set = c("High Demand", "Low Demand"); forms = c("1) Fixed Capacity", "2) Adjustible Capacity", "3) No Agreement")

# collect info for simulation distributions of realization-level summary statistics
Stats = c("Formulation", "Realization", 1:36)
f = 1
for (s in c(f1_sim, f2_sim, f0_sim)) {
  for (r in 0:499) {
    # read in realization data
    WS = read.csv(paste("demand_bootstraps/WaterSources_s", 
                        as.character(s), "_r", 
                        as.character(r), ".csv", sep = ""), header = TRUE)
    U  = read.csv(paste("demand_bootstraps/Utilities_s",
                        as.character(s), "_r",
                        as.character(r), ".csv", sep = ""), header = TRUE)
    P  = read.csv(paste("demand_bootstraps/Policies_s",
                        as.character(s), "_r",
                        as.character(r), ".csv", sep = ""), header = TRUE)
    
    # take summary stats of select variables by utility
    # policies - count frequency of weeks under restriction, take average weekly water transfer
    Pstats = c(apply(as.matrix(P[,1:6]) != 1, MARGIN = 2, sum),
               apply(as.matrix(P[,c(7:11,16,17)]), MARGIN = 2, mean))
    
    # utilities - average weekly net inflow, cumulative unfulfilled demand, total weeks with storage failure
    Ustats = c(apply(as.matrix(U[,grep("net_inf",colnames(U))]), MARGIN = 2, mean),
               apply(as.matrix(U[,grep("unfulf",colnames(U))]), MARGIN = 2, sum),
               apply((as.matrix(U[,grep("st_vol",colnames(U))]) / 
                       as.matrix(U[,grep("capacity",colnames(U))[seq(1,by = 2, to = length(grep("capacity",colnames(U))))]])) <= 0.2, 
                     MARGIN = 2, sum))
    
    # water sources - use of jordan lake storage (average weekly storage level)
    Wstats = apply(as.matrix(WS[,grep("X6alloc",colnames(WS))[c(1:3,5,6)]]), MARGIN = 2, mean)
    
    # collect stats in master set
    Stats = rbind(Stats, c(forms[f], r, Pstats, Ustats, Wstats))
  }
  f = f + 1
}
Stats = as.data.frame(Stats[2:nrow(Stats),])
colnames(Stats) = c("Formulation", "Realization", 
                    "Cary - Restrictions", "Durham - Restrictions", "OWASA - Restrictions", 
                    "Raleigh - Restrictions", "Chatham - Restrictions", "Pittsboro - Restrictions",
                    "OWASA - Jordan Lake Transfers", "Durham - Jordan Lake Transfers", "Cary - Jordan Lake Transfers", 
                    "Raleigh - Jordan Lake Transfers", "Chatham - Jordan Lake Transfers", 
                    "Chatham - Other Transfers", "Pittsboro - Other Transfers",
                    "OWASA - Net Inflow", "Durham - Net Inflow", "Cary - Net Inflow", 
                    "Raleigh - Net Inflow", "Chatham - Net Inflow", "Pittsboro - Net Inflow",
                    "OWASA - Unfulfilled Demand", "Durham - Unfulfilled Demand", "Cary - Unfulfilled Demand", 
                    "Raleigh - Unfulfilled Demand", "Chatham - Unfulfilled Demand", "Pittsboro - Unfulfilled Demand",
                    "OWASA - Storage Failure", "Durham - Storage Failure", "Cary - Storage Failure", 
                    "Raleigh - Storage Failure", "Chatham - Storage Failure", "Pittsboro - Storage Failure",
                    "OWASA - Jordan Lake Storage", "Durham - Jordan Lake Storage", "Cary - Jordan Lake Storage", 
                    "Chatham - Jordan Lake Storage", "Pittsboro - Jordan Lake Storage")

# plot by splitting facets between utility and variable, fill color by formulation, distributions of realization values
f_colors = c(wes_palette(n = 3, name = "FantasticFox1"))
SM = reshape2::melt(Stats, id = c("Formulation", "Realization"))
SM$Utility = sapply(strsplit(as.character(SM$variable), " - "), "[[", 1)
SM$Type = sapply(strsplit(as.character(SM$variable), " - "), "[[", 2)
Fplot = ggplot(data = SM) +
  geom_density(aes(x = as.numeric(as.character(value)), group = Formulation, 
                   fill = Formulation, color = Formulation), alpha = 0.5) +
  facet_wrap(Utility ~ Type, scales = "free") +
  scale_fill_manual(values = f_colors) + scale_color_manual(values = f_colors)
ggsave(plot = Fplot, paste("results_figures/F.png", sep = ""), 
       dpi = 1200, height = 10, width = 14, units = "in", bg = "transparent")

# check out realization-level transfers/restrictions/JL usage 
# especially w.r.t. Raleigh across example solutions to 
# investigate why Raleigh also benefits from WJLWTP
# NOTE: JUST LOOKING AT TRANSFERS RELATED TO JORDAN LAKE,
#   NOT THE SMALL SET BETWEEN PITTSBORO AND CHATHAM (TRANSFERS OBJECT 2)
AIcols = c("OWASA - Transfers", "Durham - Transfers", "Cary - Transfers", 
           "Raleigh - Transfers", "Chatham - Transfers", "Pittsboro - Transfers",
           "OWASA - Restrictions", "Durham - Restrictions", "Cary - Restrictions", 
           "Raleigh - Restrictions", "Chatham - Restrictions", "Pittsboro - Restrictions",
           "OWASA - Jordan Lake Storage", "Durham - Jordan Lake Storage", "Cary - Jordan Lake Storage", 
           "Chatham - Jordan Lake Storage", "Pittsboro - Jordan Lake Storage", 
           "Raleigh - Storage", "Raleigh - Inflow", "Durham - Wastewater Flow", 
           "Realization", "Formulation", "Week")
AllInfo = AIcols
VariableNames = c("X0transf", "X1transf", "X2transf", "X3transf", "X4transf", "X5transf",
                  "X0rest_m", "X1rest_m", "X2rest_m", "X3rest_m", "X4rest_m", "X5rest_m",
                  "X6alloc_0", "X6alloc_1", "X6alloc_2", "X6alloc_4", "X6alloc_5",
                  "X3st_vol", "X3net_inf", "X1wastewater")

for (s in c(f1_sim, f2_sim, f0_sim)) {
  i = 1
  for (r in c(high_real, low_real)) {
    WS = read.csv(paste("demand_bootstraps/WaterSources_s", 
                        as.character(s), "_r", 
                        as.character(r), ".csv", sep = ""), header = TRUE)
    U  = read.csv(paste("demand_bootstraps/Utilities_s",
                        as.character(s), "_r",
                        as.character(r), ".csv", sep = ""), header = TRUE)
    P  = read.csv(paste("demand_bootstraps/Policies_s",
                        as.character(s), "_r",
                        as.character(r), ".csv", sep = ""), header = TRUE)
    
    collected_info = cbind(P[,VariableNames[1:12]], WS[,VariableNames[13:17]], U[,VariableNames[18:20]])
    collected_info$Realization = r_set[i]
    collected_info$Formulation = forms[f]; collected_info$Week = 1:nrow(collected_info)
    colnames(collected_info) = 1:ncol(collected_info)
    AllInfo = rbind(AllInfo, collected_info)
    i = i + 1
  }
  f = f + 1
}
AllInfo = as.data.frame(AllInfo[2:nrow(AllInfo),])
colnames(AllInfo) = AIcols

# plot the data
AIM = reshape2::melt(AllInfo, id = c("Realization", "Formulation", "Week"))
AIM$Utility = sapply(strsplit(as.character(AIM$variable), " - "), "[[", 1)
AIM$Variable = sapply(strsplit(as.character(AIM$variable), " - "), "[[", 2)
AIM = AIM %>% select(-variable)
FigF = ggplot(data = AIM) + geom_line(aes(x = as.numeric(as.character(Week))/52, 
                                          y = as.numeric(as.character(value)), 
                                          group = Utility, color = Utility)) +
  facet_grid(Variable ~ Formulation, scales = "free_y")
ggsave(paste("results_figures/F.png", sep = ""), 
       dpi = 600, height = 8, width = 7, units = "in")
