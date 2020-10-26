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

##### ---------------------------------------------------------------------------------------
#### Figure E: Look at probabilistic pathways from all reference set solutions
####  NOTE: SOME OF THE LARGER FILES HERE ONLY NEED TO BE MADE ONCE
####        SKIP DOWN TO LATER LINES TO READ AND RUN THOSE FILES
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
formulations = c("3) No Agreement", "1) Fixed Capacity", "2) Adjustible Capacity")
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
f1_sim = 2132-1; f2_sim = 4873-1; f0_sim = 1054-1
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
PWS$Project[which(PWS$Project %in% c('WJLWTP Low - Fixed','WJLWTP Low - Variable'))] = "WJLWTP Low"
PWS$Project[which(PWS$Project %in% c('WJLWTP High - Fixed','WJLWTP High - Variable'))] = "WJLWTP High"

# move WJLWTP to its own "Utility"
PWS$Utility[which(PWS$Project %in% c('WJLWTP Low','WJLWTP High'))] = "Regional"


PWS = PWS[which(PWS$Project != "Cary WTP Upgrades"),]
E = ggplot(data = PWS) +
  geom_tile(aes(y = Project, x = as.numeric(as.character(Week))/52 + 2015, fill = Project), color = NA, 
            position = "identity", stat = "identity", alpha = 0.03, size = 0.5) + 
  theme(axis.ticks.y = element_blank(), panel.grid.major.y = element_blank(),
        panel.grid.minor.x = element_blank(), panel.grid.major.x = element_line(color = "grey90", size = 1),
        strip.background.x = element_blank(), strip.text.y = element_text(face = "bold", size = 20, angle = 0),
        strip.text.x = element_text(size = 20), panel.spacing.x = unit(1.25, "lines"),
        panel.background = element_rect(fill = "white", color = "black", size = 1.25),
        axis.title = element_text(size = 20), axis.text = element_text(size = 12),
        plot.background = element_rect(fill = "transparent", color = NA)) + guides(fill = FALSE) +
  facet_grid(Utility ~ Formulation, scales = "free_y", space = "free_y") + 
  ylab("Potential Infrastructure Project") + xlab("Year") 

# add labels referencing example solutions
f_colors = wes_palette(n = 3, name = "FantasticFox1")
PWSC = PWS[which(PWS$Utility == "Cary" & PWS$Formulation == "1) Fixed Capacity"),]
E = E + annotation_custom2(textGrob("A", gp = gpar(fontsize = 32, fontface = "bold", col = f_colors[1])),
                                xmin = 2025, xmax = 2025, ymin = 2, ymax = 2, data = PWSC)
PWSC = PWS[which(PWS$Utility == "Cary" & PWS$Formulation == "2) Adjustible Capacity"),]
E = E + annotation_custom2(textGrob("B", gp = gpar(fontsize = 32, fontface = "bold", col = f_colors[2])),
                           xmin = 2025, xmax = 2025, ymin = 2, ymax = 2, data = PWSC)
PWSC = PWS[which(PWS$Utility == "Cary" & PWS$Formulation == "3) No Agreement"),]
E = E + annotation_custom2(textGrob("C", gp = gpar(fontsize = 32, fontface = "bold", col = f_colors[3])),
                           xmin = 2025, xmax = 2025, ymin = 2, ymax = 2, data = PWSC)
Efinal_table = ggplot_gtable(ggplot_build(E))
Efinal_table$layout$clip[Efinal_table$layout$name == "panel"] = "off"
ggsave(plot = Efinal_table, paste("results_figures/E.png", sep = ""), 
       dpi = 1200, height = 10, width = 14, units = "in", bg = "transparent")



