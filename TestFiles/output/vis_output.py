import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# choose solution and realization to visualize
s = 0
r = 0

# load file
sources = pd.read_csv('WaterSources_s' + str(s) + '_r' + str(r) +'.csv')
weeks = np.arange(sources.shape[0])

# plot source 1 

# storage
# demand, inflow

