import fnmatch
import numpy as np
import pandas as pd

folder = 'TestFiles/output/'
O = 3
s = 0
r = 3

utils_file = folder + 'Utilities_s' + str(s) + '_r' + str(r) + '.out'
sources_file = folder + 'WaterSources_s' + str(s) + '_r' + str(r) + '.out'

print utils_file
print sources_file

utilities = pd.read_csv(utils_file, sep=',')
print utils_file + ' - read!'

water_sources = pd.read_csv(sources_file, sep=',')
print sources_file + ' - read!'

print utilities

# for h in water_sources.columns.values:
# 	print h

wwater_headers = fnmatch.filter(list(utilities.columns.values), '?wastewater')
wwater_headers += fnmatch.filter(list(utilities.columns.values), '??wastewater')

demand_headers = fnmatch.filter(list(water_sources.columns.values), '?demand')
demand_headers += fnmatch.filter(list(water_sources.columns.values), '??demand')

catch_headers = fnmatch.filter(list(water_sources.columns.values), '?catch_inflow')
catch_headers += fnmatch.filter(list(water_sources.columns.values), '??catch_inflow')

evaporation_headers = fnmatch.filter(list(water_sources.columns.values), '?evap')
evaporation_headers += fnmatch.filter(list(water_sources.columns.values), '??evap')

stored_volume_headers = fnmatch.filter(list(water_sources.columns.values), '?volume')
stored_volume_headers += fnmatch.filter(list(water_sources.columns.values), '??volume')

output = fnmatch.filter(list(water_sources.columns.values), '11ds_spill')

demands = water_sources[demand_headers].sum(axis='columns')
wwater = utilities[wwater_headers].sum(axis='columns')
catch = water_sources[catch_headers].sum(axis='columns')
evap = water_sources[evaporation_headers].sum(axis='columns')
vol = np.append(0, np.diff(water_sources[stored_volume_headers].sum(axis='columns')))
output = water_sources[output].sum(axis='columns')

result = catch - evap - demands + wwater - vol - output

# print water_sources[catch_headers]
for i in range(2, len(output)):
    print str(i) + '\t\t' + str(catch[i]) + '\t\t' + \
          str(demands[i]) + '\t\t' + \
          str(evap[i]) + '\t\t' + \
          str(output[i]) + '\t\t' + \
          str(vol[i]) + '\t\t' + \
          str(result[i]) + '\t\t'

