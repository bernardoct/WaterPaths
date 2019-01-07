import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
matplotlib.style.use('ggplot')
matplotlib.rcParams.update({'font.size': 8})


def plot_axis(my_df, my_columns, axes, y_labels, axis_column, weeks):
	for my, a, yl in zip(my_columns, axes[axis_column], y_labels):
		min_both = min(my_df[my][weeks])
		my_df[my][weeks].plot(ax=a, lw=1, legend=False, c='b')
		a.set_title(my)
		a.legend(['Test'], loc=0)
		a.set_xlim(weeks[0], weeks[-1])
		a.set_ylim(bottom=min(0, min_both))
		a.set_xlabel('Week')
		if axis_column == 0:
			a.set_ylabel(yl)

def plot_neuse_sources(my_df, my_columns, y_labels, weeks, name):

	my_columns = map(list, zip(*my_columns)) # transpose list of lists of axes

	fig, axes = plt.subplots(nrows=len(my_columns[0]), ncols=len(my_columns), sharex=True)
	fig.set_figheight(15)
	fig.set_figwidth(30)
	n_columns = len(my_columns)	
	axes = map(list, zip(*axes)) # transpose list of lists of axes

	for i in range(n_columns):
		plot_axis(my_df, my_columns[i], axes, y_labels, i, weeks)

	#plt.show()
	plt.savefig(name, bbox_inches='tight')

def calculate_storage_variation(df, catch_inflows, up_spill, wastewater_inflows, demand, evaporation, ds_spill):
	continuity_df = pd.DataFrame(columns=['stored_volume'])
	continuity_df['newly_stored_volume'] = df[catch_inflows] + df[up_spill] + df[wastewater_inflows]- df[demand]- df[evaporation]- df[ds_spill]

	return continuity_df

def create_lists_of_column_headers(my_sources, my_vars , vars_ids, sources_ids, y_labels_all):
	my_columns = []
	y_labels = []

	for v in vars_ids:
		my_columns_row = []
		for s in sources_ids:
			my_columns_row.append(my_sources[s] + my_vars[v])
		# print HB_columns_row
		my_columns.append(my_columns_row)

	for v in vars_ids:
		y_labels.append(y_labels_all[v])

	return my_columns, y_labels


def plot_reservoir_comparison(weeks, s, r):
	my_file = 'WaterSources_s' +str(s) +'_r' +str(r) +'.csv'

	my_sources = ['0', '1', '2', '3', '4']
	my_vars = ['volume', 'catch_inflow', 'up_spill', 'evap', 'ds_spill', 'demand', 'ww_inflow']
	y_labels_all = ['Storage [MG]', 'Catch. Inflows [MGD]', 'Upstream spillage [MGD]', 'Evaporation [MGD]', 'DS Spillage [MGD]', 'Demand [MGD]', 'WW inflow [MGD]']

	vars_ids = [0, 1, 3, 5, 4]
	sources_ids = [0, 1, 2, 3, 4]
	
	my_columns,	y_labels = create_lists_of_column_headers(my_sources, my_vars , vars_ids, sources_ids, y_labels_all)

	my_df = pd.read_csv(my_file)

	plot_neuse_sources(my_df, my_columns, y_labels, weeks, 'Res_dynamics_opt0_r' + str(r) + '.png')


def plot_utility_comparison(weeks, s, r):
	''' Input files '''
	my_file_utilities = 'Utilities_s' +str(s) +'_r' +str(r) +'.csv'
	my_file_policy = 'Policies_s' +str(s) + '_r' +str(r) + '.csv'

	''' Columns to be plotted files '''
	#HB_sources = ['Durham', 'Raleigh']
	my_sources = ['0', '1', '2']
	#HB_vars = ['ActualStorage', 'ST-ROF', 'LT-ROF', 'ActualDemand', 'UnrestrictedDemand', '_Transfers', 'RestrictionReduction']
	my_vars = ['st_vol', 'st_rof', 'lt_rof', 'rest_demand', 'unrest_demand', 'transf', 'rest_m']
	#my_vars = ['st_vol', 'st_rof', 'lt_rof', 'rest_demand', 'unrest_demand']
	y_labels_all = ['Stored vol. [MG]', 'ST-ROF [-]', 'LT-ROF [-]', 'Restricted Demand [MGD]', 'Unrest. Demand [MGD]', 'Transfered volume [MGD]', 'Restriction dem. multiplier']
	#y_labels_all = ['Stored vol. [MG]', 'ST-ROF [-]', 'LT-ROF [-]', 'Restricted Demand [MGD]', 'Unrest. Demand [MGD]']

	vars_ids = [0, 1, 2, 3, 4, 5]
	sources_ids = [0, 1, 2]
	
	my_columns,	y_labels = create_lists_of_column_headers(my_sources, my_vars , vars_ids, sources_ids, y_labels_all)

	''' Don't touch '''
	my_df_utilities = pd.read_csv(my_file_utilities)
	my_df_policy = pd.read_csv(my_file_policy)
	my_df = pd.concat([my_df_utilities, my_df_policy], axis=1)
	print my_df.head()
	plot_neuse_sources(my_df, my_columns, y_labels, weeks, 'Util_dynamics_opt0_r' + str(r) + '.png')


def compare_evaporations():
	hb_file = 'HB model results/Continuity/continuity_data_s0_r0.csv'
	my_file = 'My results/WaterSources_s0_r0.out'
	evaporation_series_hb_file = 'inputDataNewModel/evap_owasa_durham.csv'
	evaporation_series_processes_file = 'inputDataNewModel/pre_processed_files/durham_evap.csv'

	# hb_df = pd.read_csv(hb_file)
	# my_df = pd.read_csv(my_file)

	nrows = 5
	hb_evap = np.genfromtxt(evaporation_series_hb_file, delimiter=',', max_rows=nrows)
	my_evap = np.genfromtxt(evaporation_series_processes_file, delimiter=',', max_rows=nrows)

	print hb_evap.shape, my_evap.shape
	
	weeks = np.array(range(52 * 30, 52 * 40))
	# plt.plot(weeks, hb_evap[weeks]*1068, c='black', lw=2)
	# plt.plot(weeks, my_evap[weeks + 2608]*1068, c='c', lw=2)
	# plt.plot(weeks, hb_df['DurhamEvap'][weeks], c='r')
	# plt.plot(weeks, my_df['0evap'][weeks], c='b')

	for i in range(nrows):
		# plot(hb_evap, c='r')	
		plt.plot(my_evap[i], c='r', alpha=0.25)

	plt.show()


def check_restrictions(weeks, s, r):
	my_file_utilities = 'Utilities_s' +str(s) +'_r' +str(r) +'.csv'
	my_sources = ['0', '1', '2']
	my_vars = ['st_vol', 'st_rof', 'lt_rof', 'rest_demand', 'unrest_demand', 'transf', 'rest_m']




	''' Time interval to plot '''
dt = 15 # number of years
w0 = 14 # start year
weeks = range(52 * w0, 13 + 52*(w0 + dt))

s = 0 # solution
r = 9 # realization


plot_reservoir_comparison(weeks, s, r)
plot_utility_comparison(weeks, s, r)
