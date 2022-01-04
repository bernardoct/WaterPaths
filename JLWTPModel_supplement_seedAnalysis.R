# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (March 2021)
#   SUPPLEMENTAL INFO - Comparing Borg results across seeds
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid); library(png)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

##### ---------------------------------------------------------------------------------------
##### Read in pareto snapshots at 50k and the full reference set after 150k
Sets_50k = read.csv("snapshot_solutions_50k.csv", header = FALSE)
colnames(Sets_50k) = c("Formulation", "Seed",
                          "Reliability", 
                          "Restriction\nUse", 
                          "Infrastructure\nNet Present Cost", 
                          "Peak Financial\nCost",
                          "Worst-Case\nCost",
                          "Unit Cost\nof Expansion")

Pareto_150k = read.table("combined_pareto_set_150k_snapshot.csv", header = FALSE, sep = " ")
colnames(Pareto_150k) = c("Formulation", 
                          "Reliability", 
                          "Restriction\nUse", 
                          "Infrastructure\nNet Present Cost", 
                          "Peak Financial\nCost",
                          "Worst-Case\nCost",
                          "Unit Cost\nof Expansion")

##### ---------------------------------------------------------------------------------------
##### Screen for satisficing solutions in reference set and each seed
satisficing_sols_50k = which(as.numeric(as.character(Sets_50k$Reliability)) <= 0.01 & 
                             as.numeric(as.character(Sets_50k$`Restriction\nUse`)) <= 0.2 & 
                             as.numeric(as.character(Sets_50k$`Worst-Case\nCost`)) <= 0.05)

satisficing_sols_150k = which(as.numeric(as.character(Pareto_150k$Reliability)) <= 0.01 & 
                              as.numeric(as.character(Pareto_150k$`Restriction\nUse`)) <= 0.2 & 
                              as.numeric(as.character(Pareto_150k$`Worst-Case\nCost`)) <= 0.05)

Satisficing_50k = Sets_50k[satisficing_sols_50k,]
Satisficing_150k = Pareto_150k[satisficing_sols_150k,]

Satisficing_50k$Solution = 1:nrow(Satisficing_50k)
Satisficing_150k$Solution = 1:nrow(Satisficing_150k)

Satisficing_50k$Set = "50k NFE Snapshot"
Satisficing_150k$Set = "150k NFE Reference Set"

Satisficing_150k$Seed = "Reference Set"

rm(Pareto_150k, Sets_50k)

summary(as.factor(Satisficing_50k$Formulation))
summary(as.factor(Satisficing_150k$Formulation))

##### ---------------------------------------------------------------------------------------
##### Make parallel plots of each seed vs. reference in a big plot
master_set_to_plot = rbind(Satisficing_150k[,c(1,10,2:9)], Satisficing_50k)

# make traditional parallel plot of results
master_set_to_plot$Formulation = as.factor(master_set_to_plot$Formulation)
master_set_to_plot$Formulation = plyr::revalue(master_set_to_plot$Formulation, c('0'="3) No Agreement", 
                                                                                 '1'="1) Fixed Capacity         ",
                                                                                 '2'="2) Adjustable Capacity"))

colmaxes = apply(master_set_to_plot[,3:8],2,max); colmaxes = c(0.05,0.25,1000,3.5,0.15,5000)
colmins = apply(master_set_to_plot[,3:8],2,min); colmins = c(0,0,0,0,0,0)
for (r in 1:nrow(master_set_to_plot)) {
  master_set_to_plot[r,3:8] = (master_set_to_plot[r,3:8] - colmins) / (colmaxes - colmins)
}
OM = reshape2::melt(master_set_to_plot, id = c("Formulation", "Solution", "Set", "Seed"))

fi = 1
for (f in unique(OM$Formulation)[c(2,3,1)]) {
  Seeds = ggplot(data = OM[which(OM$Formulation == f),]) + 
    geom_line(aes(x = variable, y = value, group = Solution, color = Formulation)) +
    scale_color_manual(values = wes_palette(n = 3, name = "FantasticFox1")[fi]) + 
    facet_grid(Formulation ~ Seed) +
    theme(panel.background = element_rect(color = NA, fill = "transparent"),
          panel.grid.major.y = element_blank(),
          panel.grid.minor = element_blank(),
          legend.key = element_blank(),
          legend.background = element_rect(color = NA, fill = "transparent"),
          legend.position = "none",
          panel.grid.major.x = element_line(color = "black"), 
          plot.margin = unit(c(1,1,1,1), "in"),
          plot.background = element_rect(color = NA, fill = "transparent"),
          axis.text = element_blank(),
          axis.ticks.y = element_blank(), 
          axis.title = element_blank()) + 
    scale_y_continuous(expand = c(0,0)) + labs(color = "Formulation")
  
  fi = fi + 1
  # will add parallel axis labels in ppt
  ggsave(plot = Seeds, paste("results_figures/Seed_Comparisons_", f, ".png", sep = ""), 
         dpi = 400, height = 5, width = 18, units = "in", bg = "white")
}


# now plot hypervolumes

  
