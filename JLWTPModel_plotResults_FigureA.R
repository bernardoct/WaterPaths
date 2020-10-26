# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE A
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

##### ---------------------------------------------------------------------------------------
#### Figure A: set of parallel axis plots of all objectives, brushing for satisficing results
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE50000.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)

# export full reference set solution DVs for simulation
#write.table(Pareto[,8:77], "Reference_DVs.csv", sep = ",", row.names = FALSE, col.names = FALSE)

# Bernardo 2017: Reliability > 98%, Restrictions < 20%, Worst Case Costs < 40%
# HB 2016: Reliability > 99%, Restrictions < 20%, Peak Financial Risk (same as current WCC objective) < 10%
#Satisficing = Pareto[which(Pareto$Rel <= 0.01 & Pareto$RF <= 0.2 & Pareto$WCC <= 0.4),]
satisficing_sols = which(Pareto$Rel <= 0.01 & Pareto$RF <= 0.2 & Pareto$WCC <= 0.1)
#Satisficing = Pareto[satisficing_sols,]
#Satisficing$Set = "Satisficing"; Satisficing$Solution = 1:nrow(Satisficing)
# export satisficing solution DVs for simulation
#write.table(Satisficing[,8:77], "Satisficing_DVs.csv", sep = ",", row.names = FALSE, col.names = FALSE)

# make traditional parallel plot of results
Objs = Pareto[,c(1:7,78,79)]
Objs$group = as.factor(Objs$group)
Objs$group = plyr::revalue(Objs$group, c('0'="3) No Agreement", '1'="1) Fixed Capacity", '2'="2) Adjustible Capacity"))
colnames(Objs) = c("Reliability", 
                   "Restriction\nUse", 
                   "Infrastructure\nNPC", 
                   "Peak Financial\nCost",
                   "Worst-Case\nMitigation Cost",
                   "Unit Cost\nof Expansion", 
                   "Formulation", "Set", "Solution")
Objs$Satisficing = "Full Reference Set"
OS = Objs[which(Objs$Solution %in% satisficing_sols),]
OS$Satisficing = "Satisficing\nSolutions"
Objs = rbind(Objs, OS)

colmaxes = apply(Objs[,1:6],2,max); colmaxes = c(1,1,1000,3,4,4000)
colmins = apply(Objs[,1:6],2,min); colmins = c(0,0,0,0,0,0)
for (r in 1:nrow(Objs)) {
  Objs[r,1:6] = (Objs[r,1:6] - colmins) / (colmaxes - colmins)
}
OM = reshape2::melt(Objs, id = c("Formulation", "Solution", "Set", "Satisficing"))
OM_base = OM[which(OM$Satisficing == "Full Reference Set"),]
OM_base$Satisficing = "Solutions not\nmeeting criteria"
A_base = ggplot(data = OM_base) + 
  geom_line(aes(x = variable, y = value, group = Solution, color = Satisficing), alpha = 0.4) +
  scale_color_manual(values = "grey80") + 
  theme(panel.background = element_blank(),
        legend.key = element_blank(),
        panel.grid.major.x = element_line(color = "black"), 
        plot.margin = unit(c(1,1,1,1), "in"),
        axis.text = element_blank(),
        axis.ticks.y = element_blank(), 
        axis.title = element_blank()) + 
  scale_y_continuous(expand = c(0,0)) + labs(color = "Formulation")

# add text labels of objective bounds
MaxText = c("0%", "100%", "$1B", "300%", "400%", "$4/MGal")
MinText = c("100%", "0%", "$0", "0% AVR", "0% AVR", "$0/MGal")
ObjText = c("Reliability", "Use\nRestriction\nFrequency", "Infrastructure\nNPC", 
            "Peak\nFinancial\nCost", "Worst-Case\nMitigation\nCost", "Unit\nCost of\nExpansion")
xloc = c(0.5, 1, 1.5, 2, 2.5, 3) * 2
for (spot in 1:length(ObjText)) {
  A_base = A_base + 
    annotation_custom(grob = textGrob(MinText[spot], gp = gpar(fontsize = 8)),  
                      xmin = xloc[spot], xmax = xloc[spot], 
                      ymin = -0.06, ymax = -0.06) +
    annotation_custom(grob = textGrob(MaxText[spot], gp = gpar(fontsize = 8)),  
                      xmin = xloc[spot], xmax = xloc[spot], 
                      ymin = 1.06, ymax = 1.06) +
    annotation_custom(grob = textGrob(ObjText[spot], gp = gpar(fontsize = 7, fontface = "bold")),
                      xmin = xloc[spot], xmax = xloc[spot],
                      ymin = -0.3, ymax = -0.3)
}
A_base_table = ggplot_gtable(ggplot_build(A_base))
A_base_table$layout$clip[A_base_table$layout$name == "panel"] = "off"
ggsave(plot = A_base_table, "results_figures/A_base.png", dpi = 600, height = 3.5, width = 7, units = "in")

# steps for animation
library(wesanderson)
OMS = OM[which(OM$Satisficing != "Full Reference Set"),]
f_colors = c(wes_palette(n = 3, name = "FantasticFox1"), "grey80")
A_base_set = A_base
for (f in unique(OMS$Formulation)[c(2,3,1)]) {
  OM_set = OMS[which(OMS$Formulation == f),]
  A_base_set = A_base_set + 
    geom_line(data = OM_set, aes(x = variable, y = value, group = Solution, color = Formulation)) +
    scale_color_manual(values = f_colors) + 
    theme(panel.background = element_blank(),
          panel.grid.major.x = element_line(color = "black"), 
          plot.margin = unit(c(1,1,1,1), "in"),
          legend.key = element_blank(),
          axis.text = element_blank(),
          axis.ticks.y = element_blank(), 
          axis.title = element_blank()) + 
    scale_y_continuous(expand = c(0,0)) + labs(color = "Formulation")
  A_base_set_table = ggplot_gtable(ggplot_build(A_base_set))
  A_base_set_table$layout$clip[A_base_set_table$layout$name == "panel"] = "off"
  ggsave(plot = A_base_set_table, paste("results_figures/A_", f, ".png", sep = ""), dpi = 600, height = 3.5, width = 7, units = "in")
}

# quick pie chart to show ratio of satisficing solutions by formulation
num_sols_per_form = OS %>% group_by(Formulation) %>% summarize(n_sols = n()) %>% 
  mutate(FID = as.numeric(substr(as.character(Formulation), 1, 1))) %>% arrange(FID)
A_pie = ggplot(data = num_sols_per_form) + 
  geom_bar(aes(x = "", fill = as.character(FID), y = n_sols), 
           color = "white", stat = "identity", width = 1) + 
  coord_polar("y", start = 0) +
  theme_void() + theme(legend.position = "none") + 
  scale_fill_manual(values = wes_palette(n = 3, name = "FantasticFox1"))
ggsave(plot = A_pie, "results_figures/A_pie.png", dpi = 1200, units = "in", height = 2, width = 2)

# combine for single plot
library(png)
pie_img = readPNG("results_figures/A_pie.png")
A_final = A_base_set + annotation_custom(rasterGrob(pie_img, interpolate = TRUE),
                                         xmin = 0, xmax = 0.9, ymin = 0.4, ymax = 1.3)
A = ggplot_gtable(ggplot_build(A_final))
A$layout$clip[A$layout$name == "panel"] = "off"
ggsave(plot = A, "results_figures/A.png", dpi = 1200, units = "in", height = 3.5, width = 7)

