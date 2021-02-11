# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE E
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
annotation_custom2 <- function (grob, xmin = -Inf, xmax = Inf, ymin = -Inf, ymax = Inf, data) { 
  layer(data = data, stat = StatIdentity, position = PositionIdentity, 
        geom = ggplot2:::GeomCustomAnn,inherit.aes = TRUE, params = list(grob = grob, xmin = xmin, xmax = xmax, ymin = ymin, ymax = ymax))}

#### ----------------------------------------------------------------------------------------
#### cluster realizations based on demand trends
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

# group by LHS samples
whichA = which(LHS_parameter_samples[,1] > 0.1)
whichTau = which(LHS_parameter_samples[,2] < 3500)
whichp = which(LHS_parameter_samples[,3] > 1000)
whichall = Reduce(intersect, list(whichA,whichTau,whichp))

# check what these look like
check_set = reshape2::melt(multipliers[whichall,seq(1,2496,13)])
ggplot(data = check_set) + geom_line(aes(x = as.numeric(as.character(Var2)),
                                         y = as.numeric(as.character(value)),
                                         group = Var1))

# calculate sinusoidal factor value during x-year periods
periods = seq(from = 1, to = ncol(multipliers), by = 52*5); real_id = 0:(nrow(multipliers)-1)
sets = c("Realization", "Period", "Max Factor", "Min Factor")
for (i in c(2:length(periods[1:length(periods)]))) {
  real_max = as.data.frame(apply(multipliers[,periods[(i-1)]:periods[i]], MARGIN = 1, mean))
  real_min = as.data.frame(apply(multipliers[,periods[(i-1)]:periods[i]], MARGIN = 1, mean))
  sets = rbind(sets, cbind(1:nrow(multipliers), rep(periods[i-1], 500), real_max, real_min))
}
sets = as.data.frame(sets[2:nrow(sets),]); colnames(sets) = c("Realization", "Period", "Max Factor", "Min Factor")
sets$Period = as.numeric(as.character(sets$Period))
sets$`Max Factor` = as.numeric(as.character(sets$`Max Factor`))
sets$`Min Factor` = as.numeric(as.character(sets$`Min Factor`))
Smax = reshape2::dcast(data = sets, formula = Realization ~ Period, value.var = "Max Factor")
Smin = reshape2::dcast(data = sets, formula = Realization ~ Period, value.var = "Min Factor")

# try some custering to find a few sets of multiplier vars
Cs = kmeans(S[,2:ncol(S)], centers = 20)

# plot by cluster to see whats up
M = as.data.frame(multipliers); colnames(M) = 1:ncol(M)
M$Cluster = Cs$cluster; M$Realization = 1:500
MM = reshape2::melt(M, id = c("Realization", "Cluster"))
MM$variable = as.numeric(as.character(MM$variable))
MM_sub = MM[which(MM$variable %in% seq(1, max(MM$variable), by = 13)),] # just get seasonal values for plotting

EC = ggplot(data = MM_sub) + 
  geom_line(aes(x = variable, y = value, color = Cluster, group = Realization)) +
  facet_wrap(.~Cluster)

setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
ggsave(plot = EC, paste("results_figures/EC.png", sep = ""), 
       dpi = 400, height = 6, width = 6, units = "in", bg = "transparent")

# which period has highest/lowest value?
SAmin = apply(Smin[,2:ncol(Smin)], MARGIN = 1, function(x) {which.min(x)})
SAmax = apply(Smax[,2:ncol(Smax)], MARGIN = 1, function(x) {which.max(x)})
highest_late = which(SAmax == 10)
highest_early = which(SAmax == 3)
lowest_late = which(SAmin == 10)
lowest_early = which(SAmin == 3)
highest_mid = which(SAmax == 6)
lowest_mid = which(SAmin == 6)
whichall = Reduce(intersect, list(highest_early,lowest_late))
whichall = Reduce(intersect, list(lowest_early,highest_late))

high_early = which(Smin[,3] > 1)
high_late = which(Smin[,10] > 1.2)
low_early = which(Smax[,3] < 1)
low_late = which(Smax[,10] < 0.9)

whichall = Reduce(intersect, list(high_early,low_late))
whichall = Reduce(intersect, list(low_early,high_late))

# for some reason, method above not working to isolate proper realizations
# just look at specific weeks along timeseries
high_late = which(multipliers[,ncol(multipliers)] > 1.15)
low_late = which(multipliers[,ncol(multipliers)] < 0.85)
low_early_high_late = which(multipliers[,520] < 1 & multipliers[,ncol(multipliers)] > 1.15)
high_early_low_late = which(multipliers[,520] > 1 & multipliers[,ncol(multipliers)] < 0.85)
high_mid = which(multipliers[,1300] > 1.15)
low_mid = which(multipliers[,1300] < 0.85)

# check what these look like
check_set = reshape2::melt(multipliers[high_mid,seq(1,2496,13)])
ggplot(data = check_set) + geom_line(aes(x = as.numeric(as.character(Var2)),
                                         y = as.numeric(as.character(value)),
                                         group = Var1))

dem_real_subsets = list(low_early_high_late, high_early_low_late, high_mid, low_mid)

# try it manually - use derivatives to find realizations that change sign mid run
library(pspline)
derivative = matrix(NA, nrow = 500, ncol = 2496)
for (r in 1:500) {
  derivative[r,] = predict(sm.spline(1:2496, multipliers[r,]),1:2496,1)
}
changers = which(apply(derivative[,500:1500], MARGIN = 1, function(x) {min(x) < 0 & max(x) > 0}))

change_end_high = Reduce(intersect, list(changers, highest_early))
change_end_low = Reduce(intersect, list(changers, lowest_early))

# check what these look like
check_set = reshape2::melt(multipliers[change_end_low,seq(1,2496,13)])
ggplot(data = check_set) + geom_line(aes(x = as.numeric(as.character(Var2)),
                                         y = as.numeric(as.character(value)),
                                         group = Var1))




##### ---------------------------------------------------------------------------------------
#### Figure E: Look at probabilistic pathways from all reference set solutions
####  NOTE: SOME OF THE LARGER FILES HERE ONLY NEED TO BE MADE ONCE
####        SKIP DOWN TO LATER LINES TO READ AND RUN THOSE FILES
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("3) No Agreement", "1) Fixed Capacity", "2) Adjustable Capacity")
projects = c('Durham Reservoirs', "Falls Lake", "Wheeler Benson Lakes", 'Stone Quarry', # EXISTING
             'Cane Creek Reservoir', 'University Lake', 'Jordan Lake', 'Haw River Intake','Sanford Dummy Intake', # EXISTING
             'Little River Reservoir', 'Richland Creek Quarry', # NEW RALEIGH
             'Teer Quarry Expansion', # NEW DURHAM
             'Neuse River Intake', 'Harnett County Intake', # NEW RALEIGH
             'Dummy Endpoint', # MODEL DUMMY ENDPOINT NODE
             'Stone Quarry\nLow Expansion','Stone Quarry\nHigh Expansion','University Lake Expansion', # NEW OWASA
             'Lake Michie\nLow Expansion','Lake Michie\nHigh Expansion', # NEW DURHAM
             'Falls Lake Reallocation', # NEW RALEIGH
             'Reclaimed Water\nLow Build', 'Reclaimed Water\nHigh Build', # NEW DURHAM
             'Cane Creek\nReservoir Expansion', # NEW OWASA
             'Haw River Intake Low','Haw River Intake High', # NEW PITTSBORO
             'Cary WTP Upgrades', # NEW CARY
             'Sanford Intake Low','Sanford Intake High', # NEW PITTSBORO/CHATHAM
             'WJLWTP Low - Fixed','WJLWTP High - Fixed',
             'WJLWTP Low - Variable','WJLWTP High - Variable',
             'Cape Fear Intake') # NEW CARY

######### REMEMBER TO DOWNLOAD THE NEW PATHWAYS, MIGHT BE DIFFERENT FOR SOME REASON #######
############### MAKE SURE THESE ARE THE SAME AS BEING USED FOR FIGURE D ###################
f1_sim = 1361-1; f2_sim = 4623-1; f0_sim = 380-1
f1_sim = 3613-1; f2_sim = 4249-1; f0_sim = 380-1
Chosen = Pareto[c(f0_sim+1, f1_sim+1, f2_sim+1),] # confirm these are satisficing
Pathways = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
for (p in c(f1_sim, f2_sim, f0_sim)) {
  P = read.table(paste("sim_pathways/Pathways_s", as.character(p), ".out", sep = ""), 
                 sep = "\t", header = TRUE)
  P$Solution = p-1
  P$Formulation = formulations[Pareto$group[p]+1]
  
  Pathways = rbind(Pathways, P)
}
Pathways = Pathways[2:nrow(Pathways),]
colnames(Pathways) = c("Realization", "Utility", "Week", "Project", "Solution", "Formulation")
Pathways$Utility = utilities[as.numeric(as.character(Pathways$Utility))+1]
Pathways$Project = projects[as.numeric(as.character(Pathways$Project))+1]

PWS = Pathways
PWS$Project = as.character(PWS$Project)
PWS$Project[which(PWS$Project %in% c('WJLWTP Low - Fixed','WJLWTP Low - Variable'))] = "Jordan Lake\nWTP Low"
PWS$Project[which(PWS$Project %in% c('WJLWTP High - Fixed','WJLWTP High - Variable'))] = "Jordan Lake\nWTP High"

# move WJLWTP to its own "Utility"
PWS$Utility[which(PWS$Project %in% c('Jordan Lake\nWTP Low','Jordan Lake\nWTP High'))] = "Regional"

# correct reporting issue with sanford intake low
# built for both Chatham and Pittsboro, but only recorded by one utility
temp = PWS[which(PWS$Project == "Sanford Intake Low"),]
if (length(unique(temp$Utility)) == 1) {
  missing_utility = c("Chatham", "Pittsboro")[which(c("Chatham", "Pittsboro") != unique(temp$Utility))]
  temp$Utility = missing_utility
  PWS = rbind(PWS, temp)
}
PWS = PWS[which(PWS$Project != "Cary WTP Upgrades"),]
PWS$Set = "All Realizations"

# get high and low demand subsets
# select_realizations = read.csv("demand_bootstraps/old_bootstraps/bootstrap_realizations_2_100_S1.csv", header = FALSE)
# high_reals = c(t(select_realizations[1,]))
# low_reals = c(t(select_realizations[2,]))
# PWSH = PWS[which(PWS$Realization %in% high_reals),]; PWSH$Set = "High Demand"
# PWSL = PWS[which(PWS$Realization %in% low_reals),]; PWSL$Set = "Low Demand"
# recombine sets
# PWSA = rbind(PWS, PWSH, PWSL)

PWSA = PWS
dem_names = c("Low-High", "High-Low", "Mid High", "Mid Low"); i = 1
for (set in dem_real_subsets) {
  sub_set = PWS[which(PWS$Realization %in% set),]
  sub_set$Set = dem_names[i]; i = i + 1
  PWSA = rbind(PWSA, sub_set)
}
PWSAS = PWSA[which(PWSA$Set != "All Realizations"),]

#f_colors = c(wes_palette(n = 6, name = "IsleofDogs1")[c(1:3,5,6)], "blue3")
E = ggplot() +
  geom_tile(data = PWSAS[which(PWSAS$Set == "High-Low"),], aes(y = Project, x = as.numeric(as.character(Week))/52 + 2015), 
            color = NA, fill = "black",
            position = "identity", stat = "identity", alpha = 0.03, size = 0.5) + 
#  geom_step(data = PWSAS, aes(y = Project, x = as.numeric(as.character(Week))/52 + 2015, 
#                              color = Set, group = Set), size = 2, position = "dodge") +
  theme(axis.ticks.y = element_blank(), panel.grid.major.y = element_blank(),
        panel.grid.minor.x = element_blank(), panel.grid.major.x = element_line(color = "grey90", size = 1),
        strip.background.x = element_blank(), strip.text.y = element_text(face = "bold", size = 20, angle = 0),
        strip.text.x = element_text(size = 20), panel.spacing.x = unit(1.25, "lines"),
        panel.background = element_rect(fill = "white", color = "black", size = 1.25),
        axis.title = element_text(size = 20), axis.text = element_text(size = 12),
        plot.background = element_rect(fill = "transparent", color = NA)) +
  facet_grid(Utility ~ Formulation, scales = "free_y", space = "free_y") + 
#  scale_fill_manual(values = f_colors) +
  guides(fill = FALSE) +
  ylab("Potential Infrastructure Project") + xlab("Year") +
  ggtitle("Infrastructure pathways for example solutions: regional Jordan Lake WTP offsets independent development for partners")

# add labels referencing example solutions
f_colors = wes_palette(n = 3, name = "FantasticFox1")
PWSC = PWS[which(PWS$Utility == "Cary" & PWS$Formulation == "1) Fixed Capacity"),]
E = E + annotation_custom2(textGrob("A", gp = gpar(fontsize = 32, fontface = "bold", col = f_colors[1])),
                                xmin = 2025, xmax = 2025, ymin = 2, ymax = 2, data = PWSC)
PWSC = PWS[which(PWS$Utility == "Cary" & PWS$Formulation == "2) Adjustable Capacity"),]
E = E + annotation_custom2(textGrob("B", gp = gpar(fontsize = 32, fontface = "bold", col = f_colors[2])),
                           xmin = 2025, xmax = 2025, ymin = 2, ymax = 2, data = PWSC)
PWSC = PWS[which(PWS$Utility == "Cary" & PWS$Formulation == "3) No Agreement"),]
E = E + annotation_custom2(textGrob("C", gp = gpar(fontsize = 32, fontface = "bold", col = f_colors[3])),
                           xmin = 2025, xmax = 2025, ymin = 2, ymax = 2, data = PWSC)
#Efinal_table = ggplot_gtable(ggplot_build(E))
#Efinal_table$layout$clip[Efinal_table$layout$name == "panel"] = "off"
ggsave(plot = E, paste("results_figures/E_test4.png", sep = ""), 
       dpi = 400, height = 10, width = 14, units = "in", bg = "transparent")

# add animation
# library(gapminder); library(gganimate)
# Eanim = E + transition_manual(frames = Set, cumulative = FALSE) +
#   theme(plot.subtitle = element_text(size = 20, face = "bold")) +
#   ggtitle("Infrastructure pathways for example solutions: regional Jordan Lake WTP offsets independent development for partners",
#           subtitle = 'Set: {current_frame}')
# Eanim = animate(Eanim, width = 1100, height = 850)
# anim_save("results_figures/E.gif")



