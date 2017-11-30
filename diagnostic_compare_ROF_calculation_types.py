import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
from multiprocessing import Process
matplotlib.style.use('ggplot')
# matplotlib.rcParams.update({'font.size': 8})

os.chdir("C:\Users\David\OneDrive - University of North Carolina at Chapel Hill\UNC\Research\WSC\Coding\WaterPaths\TestFiles\output")

fig_id = 0

def plot_axis(hb_df, my_df, HB_columns, my_columns, axes, y_labels, axis_column, weeks):
	for hb, my, a, yl in zip(HB_columns, my_columns, axes[axis_column], y_labels):
		if hb == '':
			min_both = min(my_df[my][weeks])
			my_df[my][weeks].plot(ax=a, lw=1, legend=True, c='b')
			a.set_title(my)
			a.legend(['Mine'], loc=0)
		elif my == '':
			min_both = min(hb_df[hb][weeks])
			hb_df[hb][weeks].plot(ax=a, lw=1, legend=True, c='r')
			a.set_title(hb)
			a.legend(['HB'], loc=0)
		else:
			min_both = min(min(hb_df[hb][weeks]), min(my_df[my][weeks]))
			hb_df[hb][weeks].plot(ax=a, lw=1, legend=True)
			my_df[my][weeks].plot(ax=a, lw=1, legend=True)
			a.set_title(hb)
			a.legend(['HB', 'Mine'], loc=0)
		a.set_xlim(weeks[0], weeks[-1])
		a.set_ylim(bottom=min(0, min_both))
		a.set_xlabel('Week')
		a.set_ylabel(yl)


def plot_neuse_sources(hb_df, HB_columns, my_df, my_columns, y_labels, weeks):

	global fig_id

	HB_columns = map(list, zip(*HB_columns)) # transpose list of lists of axes
	my_columns = map(list, zip(*my_columns)) # transpose list of lists of axes

	plt.figure(fig_id)
	fig_id += 1
	fig, axes = plt.subplots(nrows=len(HB_columns[0]), ncols=len(HB_columns), sharex=True)
	n_columns = len(HB_columns)	
	axes = map(list, zip(*axes)) # transpose list of lists of axes

	for i in range(n_columns):
		plot_axis(hb_df, my_df, HB_columns[i], my_columns[i], axes, y_labels, i, weeks)

	fig.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.05)

	return axes

def add_trigger_vales_to_plot(axes, triggers_dict, HB_columns, HB_utilities_names, weeks):
	axes = map(list, zip(*axes)) # transpose list of lists of axes
	print len(axes), len(HB_columns)
	print len(axes[0]), len(HB_columns[0])
	for axs, column in zip(axes, HB_columns):
		for a, c in zip(axs, column):
			for u_name in HB_utilities_names:
				if (c == u_name + 'ST-ROF [-]'):
					a.plot(weeks, np.ones(len(weeks)) * triggers_dict[0][u_name + 'Restriction'], label='Res. trig.', c='g')
					a.plot(weeks, np.ones(len(weeks)) * triggers_dict[1][u_name + 'Transfers'], label='Tr. trig.', c='y')
					a.plot(weeks, np.ones(len(weeks)) * triggers_dict[2][u_name + 'Insurance'], label='Ins. trig.', c='m')
					a.legend(['HB', 'Mine', 'Res. trig.', 'Tr. trig.', 'Ins. trig.'], loc=0)
				if (c == u_name + 'LT-ROF [-]'):
					a.plot(weeks, np.ones(len(weeks)) * triggers_dict[3][u_name + 'InfTrigger'], label='Inf. trig.', c='g')
					a.legend(['HB', 'Mine', 'Inf. trig.'], loc=0)

def calculate_storage_variation(df, catch_inflows, up_spill, wastewater_inflows, demand, evaporation, ds_spill):
	continuity_df = pd.DataFrame(columns=['stored_volume'])
	continuity_df['newly_stored_volume'] = df[catch_inflows] + df[up_spill] + df[wastewater_inflows]- df[demand]- df[evaporation]- df[ds_spill]

	return continuity_df

def create_lists_of_column_headers(HB_sources, my_sources, HB_vars, my_vars, vars_ids, sources_ids, y_labels_all):
	HB_columns = []
	my_columns = []
	y_labels = []

	for v in vars_ids:
		HB_columns_row = []
		my_columns_row = []
		for s in sources_ids:
			HB_columns_row.append(HB_sources[s] + HB_vars[v])
			my_columns_row.append(my_sources[s] + my_vars[v])
		HB_columns.append(HB_columns_row)
		my_columns.append(my_columns_row)

	for v in vars_ids:
		y_labels.append(y_labels_all[v])

	return HB_columns, my_columns, y_labels

def plot_reservoir_comparison(weeks, s, r):
	''' Input files '''
	dg_file = 'david_ROFtable/WaterSources_s' + str(s) + '_r' + str(r) + '.out'
	bt_file = 'bernardo_ROFtable/WaterSources_s' + str(s) + '_r' + str(r) + '.out'

	''' Columns to be plotted files '''
	#dg_sources = ['Durham', 'FallsLake', 'LakeWB', 'Teer']
	bt_sources = ['0', '1', '2', '9']
	dg_sources = ['0', '1', '2', '9']
	all_vars = ['volume', 'catch_inflow', 'up_spill', 'evap', 'ds_spill', 'demand', 'ww_inflow']
	y_labels_all = ['Storage [MG]', 'Catch. Inflows [MGD]', 'Upstream spillage [MGD]', 'Evaporation [MGD]', 'DS Spillage [MGD]', 'Demand [MGD]', 'WW inflow [MGD]']

	''' What to plot '''
	vars_ids = [0, 1, 3, 5]
	sources_ids = [0, 1, 2, 3]
	
	dg_columns,	bt_columns,	y_labels = create_lists_of_column_headers(dg_sources, bt_sources, all_vars, all_vars , vars_ids, sources_ids, y_labels_all)

	''' Don't touch '''
	dg_df = pd.read_csv(dg_file)
	bt_df = pd.read_csv(bt_file)

	plot_neuse_sources(dg_df, dg_columns, bt_df, bt_columns, y_labels, weeks)


def plot_utility_comparison(weeks, s, r):
	''' Input files '''
	dg_file_utilities = 'david_ROFtable/Utilities_s' + str(s) + '_r' + str(r) + '.out'
	dg_file_policy = 'david_ROFtable/Policies_s' + str(s) + '_r' + str(r) + '.out'

	bt_file_utilities = 'bernardo_ROFtable/Utilities_s' + str(s) + '_r' + str(r) + '.out'
	bt_file_policy = 'bernardo_ROFtable/Policies_s' + str(s) + '_r' + str(r) + '.out'

	''' Columns to be plotted files '''
	#dg_sources = ['Durham', 'Owasa', 'Raleigh', 'Cary']
	dg_sources = ['1', '0', '3', '2']
	bt_sources = ['1', '0', '3', '2']
	all_vars = ['st_vol', 'st_rof', 'lt_rof', 'rest_demand', 'unrest_demand', 'transf', 'rest_m', 'cont_fund', 'ins_pout']
	y_labels_all = ['Storage [MG]', 'ST-ROF [-]', 'LT-ROF [-]', 'Restricted Demand [MGD]', 'Unrest. Demand [MGD]', 'Transfered volume [MGD]', 'Restriction dem. multiplier', 'Fund size (MM$)', 'Payouts (MM$)']
	rof_triggers = ['Restriction', 'Transfers', 'Insurance']

	restriction_triggers = {'DurhamRestriction': 0.98726, 'OwasaRestriction': 0.14672, 'RaleighRestriction': 0.86749, 'CaryRestriction': 0.095481}
	transfer_triggers = {'DurhamTransfers': 0.2257, 'OwasaTransfers': 0.96592, 'RaleighTransfers': 0.9897, 'CaryTransfers': 0.0}
	insurance_triggers = {'DurhamInsurance': 0.7891, 'OwasaInsurance': 0.3861, 'RaleighInsurance': 0.9976, 'CaryInsurance': 0.2229}
	infrastructure_triggers = {'DurhamInfTrigger': 0.0428, 'OwasaInfTrigger': 0.81933, 'RaleighInfTrigger': 0.001, 'CaryInfTrigger': 0.94135}
	triggers = [restriction_triggers, transfer_triggers, insurance_triggers, infrastructure_triggers]

	''' What to plot '''
	# vars_ids = range(5)
	vars_ids = [0, 1, 2, 5, 6, 7, 8]
	# vars_ids = [0, 5, 6]
	sources_ids = [0, 1, 2, 3]
	
	dg_columns,	bt_columns,	y_labels = create_lists_of_column_headers(dg_sources, bt_sources, all_vars, all_vars , vars_ids, sources_ids, y_labels_all)

	''' Don't touch '''
	dg_df_utilities = pd.read_csv(dg_file_utilities)
	dg_df_policy = pd.read_csv(dg_file_policy)
	dg_df = pd.concat([dg_df_utilities, dg_df_policy], axis=1)

	bt_df_utilities = pd.read_csv(bt_file_utilities)
	bt_df_policy = pd.read_csv(bt_file_policy)
	bt_df = pd.concat([bt_df_utilities, bt_df_policy], axis=1)


	axes = plot_neuse_sources(dg_df, dg_columns, bt_df, bt_columns, y_labels, weeks)
	add_trigger_vales_to_plot(axes, triggers, dg_columns, dg_sources, weeks)

	''' Time interval to plot '''
dt = 30
w0 = 0
s = 0
r = 0
weeks = np.array(range(52 * w0, 13 + 52*(w0 + dt)))

plot_reservoir_comparison(weeks, s, r)
plot_utility_comparison(weeks, s, r)

plt.show()
