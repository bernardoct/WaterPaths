import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def plot_hist(rel):
	name = 'Objectives_s' + rel + '.out'
	data = pd.load_csv(name, sep=' ')




fig, ((ax1, ax2, ax3), (ax4, ax5, ax6), (ax7, ax8, ax9)) = plt.subplots(3,3, figsize=10,10)
axes = [[ax1, ax2, ax3], [ax4, ax5, ax6], [ax7, ax8, ax9]]

utils = ['Watertown', 'Dryville', 'Fallsland']
objs = ['Reliability', ' Restriction Freq.', 'Infrastructure NPC', 'Peak Financial Cost', 'Worst Case Cost']
