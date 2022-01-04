## WaterPaths Bootstrapping Comparisons for WJLWTP Model
##  Sept 2020
## ------------------------------------------------

rm(list=ls())
setwd('C:/Users/dgorelic/OneDrive - University of North Carolina at Chapel Hill/UNC/Research/WSC/Coding/WP/WaterPaths/output/2/sinusoidal')

ObjectiveNames = c("Reliability", 
                   "Restriction Use", 
                   "Infrastructure NPC", 
                   "Peak Financial Cost",
                   "Worst-Case Cost",
                   "Unit Cost of Exp.", 
                   "Utility", "Realizations", "Set", "Solution")
Utilities = c("OWASA", "Durham", "Cary", "Raleigh", "Chatham", "Pittsboro")
AllObjs = ObjectiveNames; n_all_reals = 1000; n_objs = 6

# read in bootstrapping output from waterpaths simulation
# what sets do we have in the folder to read? assume they are all the same model form
bootstrap_tests_to_read = list.files(pattern = "bootstrap_obj*")
full_set_index = 0; subset_index = 0
for (test in bootstrap_tests_to_read) {
  n_subset_reals = strsplit(test, split = "_")[[1]][4]
  n_sets = strsplit(test, split = "_")[[1]][3]
  solution = strsplit(test, split = "_")[[1]][5]
  
  # skip small test sets
  if (as.numeric(n_sets) < 10 || as.numeric(n_subset_reals) < 100) {next}
  
  Full_Objs = read.csv(paste('objectives_all_reals', n_sets, n_subset_reals, 'S0.csv', sep = "_"), header = FALSE)
  Set_Objs = read.csv(paste('bootstrap_objs', n_sets, n_subset_reals, 'S0.csv', sep = "_"), header = FALSE)
  Reals_Sets = read.csv(paste('bootstrap_realizations', n_sets, n_subset_reals, 'S0.csv', sep = "_"), header = FALSE)
  
  # also skip those with old model version (not all objectives checked)
  if (ncol(Set_Objs) < length(Utilities)*n_objs) {next}
  
  for (set in 1:nrow(Set_Objs)) {
    # get results from each bootstrapped set, normalized by 1,000 realization values
    Set_Objs[set,] = Set_Objs[set,] - Full_Objs
    AllObjs = rbind(AllObjs, rbind(c(as.numeric(Set_Objs[set,1:6]), Utilities[1], n_subset_reals, subset_index, solution),
                                   c(as.numeric(Set_Objs[set,7:12]), Utilities[2], n_subset_reals, subset_index, solution),
                                   c(as.numeric(Set_Objs[set,13:18]), Utilities[3], n_subset_reals, subset_index, solution),
                                   c(as.numeric(Set_Objs[set,19:24]), Utilities[4], n_subset_reals, subset_index, solution),
                                   c(as.numeric(Set_Objs[set,25:30]), Utilities[5], n_subset_reals, subset_index, solution),
                                   c(as.numeric(Set_Objs[set,31:36]), Utilities[6], n_subset_reals, subset_index, solution)))
    subset_index = subset_index + 1
  }
  
  # collect objective results from full realization runs (1,000 realizations)
  Full_Objs = Full_Objs - Full_Objs
  AllObjs = rbind(AllObjs, rbind(c(as.numeric(Full_Objs[1:6]), Utilities[1], n_all_reals, full_set_index, solution),
                                 c(as.numeric(Full_Objs[7:12]), Utilities[2], n_all_reals, full_set_index, solution),
                                 c(as.numeric(Full_Objs[13:18]), Utilities[3], n_all_reals, full_set_index, solution),
                                 c(as.numeric(Full_Objs[19:24]), Utilities[4], n_all_reals, full_set_index, solution),
                                 c(as.numeric(Full_Objs[25:30]), Utilities[5], n_all_reals, full_set_index, solution),
                                 c(as.numeric(Full_Objs[31:36]), Utilities[6], n_all_reals, full_set_index, solution)))
  full_set_index = full_set_index + 1
}


# plot the results, normalized by the real set
AO = as.data.frame(AllObjs[2:nrow(AllObjs),])
colnames(AO) = ObjectiveNames
library(reshape2)
Am = melt(AO, id = c("Set", "Realizations", "Utility", "Solution"))

library(ggplot2)
a = ggplot(data = Am) + geom_boxplot(aes(x = Realizations, 
                                         y = as.numeric(as.character(value)), 
                                      group = Realizations, 
                                      color = Realizations)) + 
  facet_wrap(Utility ~ variable, scales = "free_y", ncol = 6, nrow = 6) + 
  ylab("Difference between (a) 1,000 Realization and (b) Sub-Set Objective Values")
ggsave("bootstrapping_comps.png", dpi = 500, height = 9, width = 14, units = "in")


