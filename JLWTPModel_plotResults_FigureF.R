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
f1_sim = 2132-1; f2_sim = 4873-1; f0_sim = 1054-1
high_real = 116; low_real = 366
f = 1; r_set = c("High Demand", "Low Demand"); forms = c("1) Fixed Capacity", "2) Adjustible Capacity", "3) No Agreement")
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
