# Results Figures for WJLWTP Paper from Borg Results
# D Gorelick (Oct 2020)
#   FIGURE A
# --------------------------------------------------

rm(list=ls())
library(dplyr); library(wesanderson); library(ggplot2); library(gridExtra); library(grid); library(png)
setwd("C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/BorgOutput")

##### ---------------------------------------------------------------------------------------
#### Figure A: set of parallel axis plots of all objectives, brushing for satisficing results
Pareto = read.csv("combined_pareto_front_by_formulation_withDVs_NFE150000.csv", header = TRUE)
Pareto$Set = "Pareto"; Pareto$Solution = 1:nrow(Pareto)

# export full reference set solution DVs for simulation
write.table(Pareto[,8:77], "Reference_DVs.csv", sep = ",", row.names = FALSE, col.names = FALSE)

# Bernardo 2017: Reliability > 98%, Restrictions < 20%, Worst Case Costs < 40%
# HB 2016: Reliability > 99%, Restrictions < 20%, Peak Financial Risk (same as current WCC objective) < 10%
#Satisficing = Pareto[which(Pareto$Rel <= 0.01 & Pareto$RF <= 0.2 & Pareto$WCC <= 0.4),]
satisficing_sols = which(as.numeric(as.character(Pareto$Rel)) <= 0.02 & 
                           as.numeric(as.character(Pareto$RF)) <= 0.2 & 
                              as.numeric(as.character(Pareto$WCC)) <= 0.1)
Satisficing = Pareto[satisficing_sols,]
Satisficing$Set = "Satisficing"; Satisficing$Solution = 1:nrow(Satisficing)
# export satisficing solution DVs for simulation
write.table(Satisficing[,8:77], "Satisficing_DVs.csv", sep = ",", row.names = FALSE, col.names = FALSE)

# make traditional parallel plot of results
Objs = Pareto[,c(1:7,78,79)]
Objs$group = as.factor(Objs$group)
Objs$group = plyr::revalue(Objs$group, c('0'="3) No Agreement", 
                                         '1'="1) Fixed Capacity         ", 
                                         '2'="2) Adjustable Capacity"))
colnames(Objs) = c("Reliability", 
                   "Restriction\nUse", 
                   "Infrastructure\nNet Present Cost", 
                   "Peak Financial\nCost",
                   "Worst-Case\nCost",
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
OM_base$Satisficing = "Solutions not\nmeeting criteria             "
A_base = ggplot(data = OM_base) + 
  geom_line(aes(x = variable, y = value, group = Solution, color = Satisficing)) +
  scale_color_manual(values = "grey90") + 
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
  scale_y_continuous(expand = c(0,0)) + labs(color = "Formulation")

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
    annotation_custom(grob = textGrob(ObjText[spot], gp = gpar(fontsize = 6, fontface = "italic"), rot = 60),
                      xmin = xloc[spot]-0.2, xmax = xloc[spot]-0.2,
                      ymin = -0.3, ymax = -0.3)
}
A_base_table = ggplot_gtable(ggplot_build(A_base))
A_base_table$layout$clip[A_base_table$layout$name == "panel"] = "off"
ggsave(plot = A_base_table, "results_figures/A_base.png", 
       dpi = 1200, height = 3.5, width = 7, units = "in", bg = "transparent")

# steps for animation
OMS = OM[which(OM$Satisficing != "Full Reference Set"),]
f_colors_list = list(c(wes_palette(n = 1, name = "FantasticFox1"), "grey90"),
                     c(wes_palette(n = 2, name = "FantasticFox1"), "grey90"),
                     c(wes_palette(n = 3, name = "FantasticFox1"), "grey90"))

A_base_set = A_base; i = 1
for (f in unique(OMS$Formulation)[c(2,3,1)]) {
  OM_set = OMS[which(OMS$Formulation == f),]
  A_base_set = A_base_set + 
    geom_line(data = OM_set, aes(x = variable, y = value, group = Solution, color = Formulation),
              alpha = 0.7) +
    scale_color_manual(values = f_colors_list[[i]]) + 
    theme(panel.background = element_rect(color = NA, fill = "transparent"),
          panel.grid.major.x = element_line(color = "black"), 
          plot.margin = unit(c(1,1,1,1), "in"),
          legend.key = element_blank(),
          legend.background = element_rect(color = NA, fill = "transparent"),
          panel.grid.major.y = element_blank(),
          panel.grid.minor = element_blank(),
          plot.background = element_rect(color = NA, fill = "transparent"),
          axis.text = element_blank(),
          axis.ticks.y = element_blank(), 
          axis.title = element_blank()) + 
    scale_y_continuous(expand = c(0,0)) + labs(color = "Formulation")
  A_base_set_table = ggplot_gtable(ggplot_build(A_base_set))
  A_base_set_table$layout$clip[A_base_set_table$layout$name == "panel"] = "off"
  ggsave(plot = A_base_set_table, paste("results_figures/A_", f, ".png", sep = ""), 
         dpi = 1200, height = 3.6, width = 7, units = "in", bg = "transparent")
  i = i + 1
}

# quick pie chart to show ratio of satisficing solutions by formulation
num_sols_per_form = OS %>% group_by(Formulation) %>% summarize(n_sols = n()) %>% 
  mutate(FID = as.numeric(substr(as.character(Formulation), 1, 1))) %>% arrange(FID)
A_pie = ggplot(data = num_sols_per_form) + 
  geom_bar(aes(x = "", fill = as.character(FID), y = n_sols), 
           color = "white", stat = "identity", width = 1) + 
  coord_polar("y", start = 0) +
  theme_void() + theme(legend.position = "none", 
                       plot.background = element_rect(fill = "transparent", color = NA)) + 
  scale_fill_manual(values = wes_palette(n = 3, name = "FantasticFox1"))
ggsave(plot = A_pie, "results_figures/A_pie.png", dpi = 1200, 
       units = "in", height = 2, width = 2, bg = "transparent")

# combine for single plot
pie_img = readPNG("results_figures/A_pie.png")
A_final = A_base_set + annotation_custom(rasterGrob(pie_img, interpolate = TRUE),
                                         xmin = -0.3, xmax = 0.9, ymin = 0.1, ymax = 1.5) +
  annotation_custom(grob = textGrob("Fraction of\nsolutions\nmeeting\ncriteria", gp = gpar(fontsize = 5)),  
                    xmin = 0.3, xmax = 0.3, ymin = 1.1, ymax = 1.1)
A = ggplot_gtable(ggplot_build(A_final))
A$layout$clip[A$layout$name == "panel"] = "off"
ggsave(plot = A, "results_figures/A.png", 
       dpi = 1200, units = "in", height = 3.6, width = 7, bg = "transparent")


# add to final plot example solutions
f1_sim = 3613-1; f2_sim = 4249-1; f0_sim = 380-1
Chosen = OM_base[which(OM_base$Solution %in% c(f1_sim+1, f2_sim+1, f0_sim+1)),]
Chosen$Satisficing = "example"
f_colors = wes_palette(n = 3, name = "FantasticFox1")
A_final2 = A_final + 
  geom_line(data = Chosen, aes(x = variable, y = value, group = Solution, 
                               color = Formulation), size = 2, linetype = "F1", alpha = 1) +
  annotation_custom(grob = textGrob("A", gp = gpar(fontsize = 15, fontface = "bold", col = f_colors[1])),
                    xmin = 4.6, xmax = 4.6,
                    ymin = 0.35, ymax = 0.35) +
  annotation_custom(grob = textGrob("B", gp = gpar(fontsize = 15, fontface = "bold", col = f_colors[2])),
                    xmin = 5.7, xmax = 5.7,
                    ymin = 0.25, ymax = 0.25) +
  annotation_custom(grob = textGrob("C", gp = gpar(fontsize = 15, fontface = "bold", col = f_colors[3])),
                    xmin = 2.55, xmax = 2.55,
                    ymin = 0.6, ymax = 0.6)
A = ggplot_gtable(ggplot_build(A_final2))
A$layout$clip[A$layout$name == "panel"] = "off"
ggsave(plot = A, "results_figures/AD_combined.png", 
       dpi = 1200, units = "in", height = 3.6, width = 7, bg = "transparent")
  
  
