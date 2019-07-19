import numpy as np
from glob import glob
from multiprocessing import Pool
from os.path import dirname
import matplotlib.pyplot as plt

def trim_file(f):
	print 'Loading ' + f
	data = np.loadtxt(f, delimiter=',')
	print f, 'old shape', data.shape
	np.savetxt(f, data[:, 262:], delimiter=',', fmt='%0.6f')
	print f, 'save with shape', data[:, 262:].shape

folders = ['evaporation/', 'inflows/', 'demands/']

files = []
for fo in folders:
	files += glob(fo + '*')


# trim_file(files[0])
Pool(8).map(trim_file, files)