# -*- coding: utf-8 -*-
"""
Created on Fri Feb 14 13:13:32 2020

@author: dgold
"""

import pandas as pd
import numpy as np
from matplotlib import pyplot as plt


'''
How to use: 
    1. run waterpaths without rof tables (set C flag to 0)
    
    2. after it has run, rename the folder called output to "no_tables_output"
    
    3. make a new folder called "output" that is empty

    4. rerun water paths with the tables  (ie first run with C flag as 1, then -1)

    5. rename the "output" folder as "tables_output"

    6. past this script in the main waterpaths working directory (above the two output folders)

    7. run the script. With default settings it will generate a plot for the first
    five years of the simulation for utility 0. To change this, just change the
    utility, start week and end week variables on lines 130-135. 
'''
def compare_output(s, r, u, start_week, end_week):
    
    # extract time series of model output with tables
    ###########################################################################
    # read tables files
    utility_data_tables = pd.read_csv('tables_output/Utilities_s' + str(s) + \
                                      '_r' + str(r) + '.csv', sep=',')
    policy_data_tables = pd.read_csv('tables_output/Policies_s' + str(s) + \
                                     '_r' + str(r) + '.csv', sep=',')
    
    # assign variables
    storage_tables = utility_data_tables[str(u) + 'st_vol'].iloc[\
                                         start_week:end_week]
    
    inflow_tables = utility_data_tables[str(u) + 'net_inf'].iloc[\
                                        start_week:end_week]
    capacity_tables = utility_data_tables[str(u) + 'capacity'].iloc[0]
    evap_tables = utility_data_tables[str(u) + 'net_inf'].iloc[\
                                      start_week:end_week]
    rest_demand_tables = utility_data_tables[str(u) + \
                               'rest_demand'].iloc[start_week:end_week]
    unrest_demand_tables = utility_data_tables[str(u) + \
                                 'unrest_demand'].iloc[start_week:end_week]
    ROF_tables = utility_data_tables[str(u) + 'st_stor_rof'].iloc[\
                                     start_week:end_week]
    
    transfers_tables = policy_data_tables[str(u) + 'transf'].iloc[\
                                          start_week:end_week]
    ###########################################################################
    
    # now extract the data from no tables
    ###########################################################################
    # read tables files
    utility_data_no_tables = pd.read_csv('no_tables_output/Utilities_s' + \
                                str(s) + '_r' + str(r) + '.csv', sep=',')
    policy_data_no_tables = pd.read_csv('no_tables_output/Policies_s' + str(s) + \
                                     '_r' + str(r) + '.csv', sep=',')
    
    # assign variables
    storage_no_tables = utility_data_no_tables[str(u) + 'st_vol'].iloc[\
                                            start_week:end_week]
    
    inflow_no_tables = utility_data_no_tables[str(u) + 'net_inf'].iloc[\
                                           start_week:end_week]
    capacity_no_tables = utility_data_no_tables[str(u) + 'capacity'].iloc[0]
    evap_no_tables = utility_data_no_tables[str(u) + 'net_inf'].iloc[\
                                         start_week:end_week]
    rest_demand_no_tables = utility_data_no_tables[str(u) + \
                               'rest_demand'].iloc[start_week:end_week]
    unrest_demand_no_tables = utility_data_no_tables[str(u) + \
                                 'unrest_demand'].iloc[start_week:end_week]
    ROF_no_tables = utility_data_no_tables[str(u) + 'st_stor_rof'].iloc[\
                                        start_week:end_week]
    
    transfers_no_tables = policy_data_no_tables[str(u) + 'transf'].iloc[\
                                             start_week:end_week]
    ###########################################################################
    
    #create plots (blue is with tables, red without)
    fig, axes = plt.subplots(nrows=4, ncols=1, figsize=(15,10))
    
    # first plot the total utility storage
    axes[0].plot(range(start_week, end_week), storage_tables, color='blue')
    axes[0].plot(range(start_week, end_week), storage_no_tables, color='red')
    axes[0].set_ylim([0, 1.2*capacity_tables]) 
    axes[0].set_title('Storage')
    
    # third plot demand
    axes[1].plot(range(start_week, end_week), unrest_demand_tables, color = \
        'blue')
    axes[1].plot(range(start_week, end_week), rest_demand_tables, color = \
        'blue', linestyle='--')
    
    axes[1].plot(range(start_week, end_week), unrest_demand_no_tables, color \
        = 'red')
    axes[1].plot(range(start_week, end_week), rest_demand_no_tables, color = \
        'red', linestyle='--')
    axes[1].set_title('Demand')
    
    # fourth plot has ROFs
    axes[2].plot(range(start_week, end_week), ROF_tables, color = 'blue')
    axes[2].plot(range(start_week, end_week), ROF_no_tables, color = 'red')
    axes[2].set_title('ROFs')

    
    # fifth plot has transfers
    axes[3].bar(range(start_week, end_week), transfers_tables, color='blue')
    axes[3].bar(range(start_week, end_week), transfers_no_tables, color='red')
    axes[3].set_title('Transfers')

    for ax in axes:
        ax.set_xlim([start_week, end_week])
    
    plt.tight_layout()
    plt.savefig('diagnostics'+ str(utility) +'_s' + str(s) +'_r ' + str(r) + \
                '.png', bbox_inches='tight')
    
    
# run for one utility
import os
os.chdir('C:\\Users\\dgorelic\\OneDrive - University of North Carolina at Chapel Hill\\UNC\\Research\\WSC\\Coding\\WP\\WaterPaths')

utility = 5
realization = 10
solution = 0
    
start_week = 0
end_week = 2344
for realization in range(15):
    compare_output(solution, realization, utility, start_week, end_week)
