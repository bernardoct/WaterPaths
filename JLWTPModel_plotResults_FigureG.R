# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Nov 2020)
#   FIGURE G
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid)

##### ---------------------------------------------------------------------------------------
#### Figure G: animated example of demand growth realizations showing influence of sinusoidal factors
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

# read demands from one utility for an example - Durham
DD = read.csv(paste("synthetic_demands/", "Durham", "_synthetic_demands_sinusoidal.csv", sep = ""), header = FALSE)
DP = read.csv(paste("annual_demand_projections_avgMGD/", "Durham", "_annual_demand_projections_MGW.csv", sep = ""), header = FALSE)
DP = as.numeric(as.character(DP$V1)); DP = as.data.frame(DP[1:48]) # trim to 48 years

# for paper - make explanatory figure with 3 example realizations of demand
example_realizations = c(308,164) # low, high
DD = DD[example_realizations,]
Ms = multipliers[example_realizations,]

# combine data for plotting
DD = as.data.frame(t(DD))
colnames(DD) = 1:ncol(DD)
DD$Week = 1:nrow(DD);DDM = reshape2::melt(DD, id = "Week")

DP$Week = 1:48 * 52 - 51; DP$variable = as.factor(0)
DPM = DP[,c(2,3,1)]; colnames(DPM)[3] = "value"

#DPM2 = DPM; DPM2$variable = as.factor(501)
#DPM = rbind(DPM, DPM2)

M = as.data.frame(t(Ms))
colnames(M) = 1:ncol(M)
M$Week = 1:nrow(M);MM = reshape2::melt(M, id = "Week")

# data without factor
DW = DD; DW[,1:2] = DW[,1:2]/M[,1:2]
DWM = reshape2::melt(DW, id = "Week")

# remove some data points to speed up plotting
#DDM = DDM[which(as.numeric(as.character(DDM$Week)) %in% seq(1, max(DDM$Week), by = 13)),]
#MM = MM[which(as.numeric(as.character(MM$Week)) %in% seq(1, max(MM$Week), by = 13)),]

DDM$Type = "Modeled (Weekly)"; DPM$Type = "Projected (Annual)"; MM$Type = "Modeled (Weekly)"
DWM$Type = "Modeled (Weekly)"
DDM$Set = "B) With Multiplier"; DPM$Set = "A) Without Multiplier"; 
MM$Set = "A) Without Multiplier"
DWM$Set = "A) Without Multiplier"

DPM2 = DPM; DPM2$Set = "B) With Multiplier"

DPM$Set2 = "Demand (MGW)"; DPM2$Set2 = "Demand (MGW)"; DDM$Set2 = "Demand (MGW)"; DWM$Set2 = "Demand (MGW)"
MM$Set2 = "Sinusoidal Factor"

#DDM$Sizer = 0.4; DPM$Sizer = 1; MM$Sizer = 0.4
#DDM$Alpha = 1; DPM$Alpha = 1; MM$Alpha = 1

AM = rbind(DPM, DPM2, DDM, MM, DWM)
AM$Realization = AM$variable

AM$variable = as.numeric(as.character(AM$variable))
AM$Week = as.numeric(as.character(AM$Week))
AM$value = as.numeric(as.character(AM$value))
#AM$Realization = as.numeric(as.character(AM$Realization))


# plot the figure
AMT = AM[which(AM$Week < 2392 & AM$Type != "Projected (Annual)"),]
AMP = AM[which(AM$Week < 2392 & AM$Type == "Projected (Annual)"),]
f_color_durham = wes_palette(n = 6, name = "IsleofDogs1")[3]
G = ggplot() + 
  geom_line(data = AMT, aes(x = Week/52 + 2015, y = value, 
                            group = variable, linetype = Type, color = Realization), 
            size = 0.65) + 
  geom_line(data = AMP, aes(x = Week/52 + 2015, y = value, 
                            group = variable, linetype = Type, color = Realization), 
            size = 2) +
  facet_grid(Set2~Set, scales = "free_y", drop = TRUE, switch = "y") + 
  guides(size = FALSE, alpha = FALSE, color = FALSE) + 
  scale_color_manual(values = c("black", "grey50", "blue3")) + 
  theme(legend.position = c(0.03, 0.97), legend.justification = c(0,1),
        legend.background = element_rect(fill = "white", colour = "black"),
        legend.text = element_text(size = 12, face = "bold"),
        plot.background = element_rect(fill = "transparent", color = NA),
        panel.background = element_rect(fill = "white", color = "black"),
        strip.background = element_blank(), strip.text = element_text(size = 15),
        axis.title.y = element_blank(),
        legend.key.width = unit(2.3, "line"),
        strip.placement.y = "outside",
        axis.text = element_text(size = 11, color = "black"),
        axis.title.x = element_text(size = 12, color = "black"),
        legend.title = element_blank(),
        legend.key = element_rect(fill = NA, color = NA),
        panel.grid.major = element_line(color = "grey90", size = 0.5),
        panel.grid.minor = element_blank()) +
  xlab("Year")
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
ggsave("results_figures/G.png", dpi = 1200, unit = "in", width = 7, height = 7, bg = "transparent")




library(gganimate)

AMT = AM[which(AM$Week < 2392),]
#AMTT = AMT[which(AMT$variable < 25),]
G = ggplot(data = AMT, aes(x = Week/52 + 2015, y = value, 
                           group = variable, color = Type)) + 
  geom_line(aes(size = as.numeric(as.character(Sizer)), 
                alpha = as.numeric(as.character(Alpha)))) + 
  facet_wrap(.~Set, scales = "free_y") + guides(size = FALSE, alpha = FALSE) + 
  scale_color_manual(values = c(f_color_durham, "black", "grey60")) + 
  theme(legend.position = c(0.03, 0.97), legend.justification = c(0,1),
        legend.background = element_rect(fill = "white", colour = "black"),
        legend.text = element_text(size = 12, face = "bold"),
        plot.background = element_rect(fill = "transparent", color = NA),
        panel.background = element_rect(fill = "white", color = "black"),
        strip.background = element_blank(), strip.text = element_text(size = 15),
        axis.title.y = element_blank(),
        axis.text = element_text(size = 11, color = "black"),
        axis.title.x = element_text(size = 12, color = "black"),
        legend.title = element_blank(),
        legend.key = element_rect(fill = NA, color = NA),
        panel.grid.major = element_line(color = "grey90", size = 0.5),
        panel.grid.minor = element_blank()) +
#  scale_size_manual(values = c(1,1,1)) +
  xlab("Year") + ggtitle("Realizations of demand growth for Durham:\nWeekly demands vs. weekly sinusoidal multipliers")

setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
#ggsave("results_figures/G.png", dpi = 1200, unit = "in", width = 9, height = 5, bg = "transparent")

GG = G + transition_manual(cumulative = TRUE, frames = Realization)
Ganim = animate(GG, width = 700, height = 350, bg = "transparent")
anim_save("results_figures/G.gif")












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