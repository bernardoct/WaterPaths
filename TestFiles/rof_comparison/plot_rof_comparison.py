import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
matplotlib.style.use('ggplot')
matplotlib.rcParams.update({'font.size': 8})

def plot_axis(my_df, my_columns, axes, y_labels, axis_column, weeks, color):
	for my, a, yl in zip(my_columns, axes[axis_column], y_labels):
		min_both = min(my_df[my][weeks])
		my_df[my][weeks].plot(ax=a, lw=1, legend=False, c=color)
		a.set_title(my)
		a.legend(['With Tables', 'Without Tables'], loc=0)
		a.set_xlim(weeks[0], weeks[-1])
		a.set_ylim(bottom=min(0, min_both))
		a.set_xlabel('Week')
		if axis_column == 0:
			a.set_ylabel(yl)


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

def plot_neuse_sources(table_df, no_table_df, my_columns, y_labels, weeks, name):

	my_columns = map(list, zip(*my_columns)) # transpose list of lists of axes

	fig, axes = plt.subplots(nrows=len(my_columns[0]), ncols=len(my_columns), sharex=True)
	fig.set_figheight(15)
	fig.set_figwidth(30)
	n_columns = len(my_columns)	
	axes = map(list, zip(*axes)) # transpose list of lists of axes

	for i in range(n_columns):
		plot_axis(table_df, my_columns[i], axes, y_labels, i, weeks, 'b')
		plot_axis(no_table_df, my_columns[i], axes, y_labels, i, weeks, 'r')

	#plt.show()
	plt.savefig(name, bbox_inches='tight')


def plot_utility_comparison(weeks, s, r):
	''' Input files '''
	table_utilities_file = 'tables/Utilities_s' +str(s) +'_r' +str(r) +'.csv'
	table_policy_file = 'tables/Policies_s' +str(s) + '_r' +str(r) + '.csv'
	no_table_utilities_file = 'no_tables/Utilities_s' +str(s) +'_r' +str(r) +'.csv'
	no_table_policy_file = 'no_tables/Policies_s' +str(s) + '_r' +str(r) + '.csv'

	''' Columns to be plotted files '''
	my_sources = ['0', '1', '2']
	my_vars = ['st_vol', 'st_rof', 'lt_rof', 'rest_demand', 'unrest_demand', 'transf', 'rest_m']
	y_labels_all = ['Stored vol. [MG]', 'ST-ROF [-]', 'LT-ROF [-]', 'Restricted Demand [MGD]', 'Unrest. Demand [MGD]', 'Transfered volume [MGD]', 'Restriction dem. multiplier']

	vars_ids = [0, 1, 2, 3, 4, 5]
	sources_ids = [0, 1, 2]
	
	my_columns,	y_labels = create_lists_of_column_headers(my_sources, my_vars , vars_ids, sources_ids, y_labels_all)

	''' create dfs from table and no table output '''
	table_utilities = pd.read_csv(table_utilities_file)
	table_policy = pd.read_csv(table_policy_file)
	table_df = pd.concat([table_utilities, table_policy], axis=1)
	no_table_utilities = pd.read_csv(no_table_utilities_file)
	no_table_policy = pd.read_csv(no_table_policy_file)
	no_table_df = pd.concat([no_table_utilities, no_table_policy], axis=1)


	plot_neuse_sources(table_df, no_table_df, my_columns, y_labels, weeks, 'Util_dynamics_opt0_r' + str(r) + '.png')



	''' Time interval to plot '''
dt = 15 # number of years
w0 = 14 # start year
weeks = range(52 * w0, 13 + 52*(w0 + dt))

s = 0 # solution
r = 9 # realization

plot_utility_comparison(weeks, s, r)