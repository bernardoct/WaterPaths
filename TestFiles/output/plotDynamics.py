import glob
import matplotlib.pyplot as plt
import numpy as np
from multiprocessing import Pool


def read_append_file(f):
    print f
    return np.loadtxt(f, delimiter=',')


p = Pool(8)

# 0 Av_vol.
# 1 Demands
# 2 Up_spil
# 3 Catc_Q
# 4 Out_Q
# watersources = []
# for f in glob.glob('WaterSources.out*'):
# 	print f
# 	watersources.append(np.loadtxt(f), delimiter=',')

# 0 Volume
# 1 ROF
# 2 Demand
# 3 Demand
# 4 Fund
# 5 Payout
# 6 Price
realizations_utilities = p.map(read_append_file, glob.glob('Utilities.out*'))

policies = p.map(read_append_file, glob.glob('Policies.out*'))

alpha = 1.0 / len(policies) + 0.004
# alpha = 0.005
# print 'alpha=' + str(alpha)

fig, (axo, axd, axc, axr) = plt.subplots(4, 1, sharex=True)
fig.suptitle('Combined Storage [MG]')
for u in realizations_utilities:
    axc.fill_between(range(u[:, 14].size), 0, u[:, 14], facecolor='red',
                     alpha=alpha)
axc.set_ylabel('Cary')
for u in realizations_utilities:
    axd.fill_between(range(u[:, 7].size), 0, u[:, 7], facecolor='red',
                     alpha=alpha)
axd.set_ylabel('Durham')
for u in realizations_utilities:
    axo.fill_between(range(u[:, 0].size), 0, u[:, 0], facecolor='red',
                     alpha=alpha)
axo.set_ylabel('OWASA')
for u in realizations_utilities:
    axr.fill_between(range(u[:, 21].size), 0, u[:, 21], facecolor='red',
                     alpha=alpha)
axr.set_ylabel('Raleigh')
axr.set_xlabel('Time [weeks]')

plt.show()

# 0 1 2 3 4 5 6 7
# O D C R 			water sources
# O D C R 			restrictions
#         O D C R 	transfers

fig2, (axo2, axd2, axc2, axr2) = plt.subplots(4, 1, sharex=True)
fig2.suptitle('ROF (red), Restriction Multiplier (blue) and Transfers (green)')
for u, p in zip(realizations_utilities, policies):
    axc2.fill_between(range(u[:, 15].size), 0, u[:, 15], facecolor='red',
                      alpha=alpha)  # ROF
    axc2.fill_between(range(p[:, 2].size), p[:, 2], 1, facecolor='blue',
                      alpha=alpha)  # Restrictions
    axc2.fill_between(range(p[:, 6].size), 0.5, -p[:, 6] / 1000 + 0.5,
                      facecolor='green', alpha=alpha)  # Transfers
axc2.set_ylabel('Cary')
axc2.set_ylim([0, 1])
for u, p in zip(realizations_utilities, policies):
    axd2.fill_between(range(u[:, 8].size), 0, u[:, 8], facecolor='red',
                      alpha=alpha)
    axd2.fill_between(range(p[:, 1].size), p[:, 1], 1, facecolor='blue',
                      alpha=alpha)
    axd2.fill_between(range(p[:, 5].size), 0.5 - p[:, 5] / 1000, 0.5,
                      facecolor='green', alpha=alpha)
axd2.set_ylabel('Durham')
axd2.set_ylim([0, 1])
for u, p in zip(realizations_utilities, policies):
    axo2.fill_between(range(u[:, 1].size), 0, u[:, 1], facecolor='red',
                      alpha=alpha)
    axo2.fill_between(range(p[:, 0].size), p[:, 0], 1, facecolor='blue',
                      alpha=alpha)
    axo2.fill_between(range(p[:, 4].size), 0.5 - p[:, 4] / 1000, 0.5,
                      facecolor='green', alpha=alpha)
axo2.set_ylabel('OWASA')
axo2.set_ylim([0, 1])
for u, p in zip(realizations_utilities, policies):
    axr2.fill_between(range(u[:, 22].size), 0, u[:, 22], facecolor='red',
                      alpha=alpha)
    axr2.fill_between(range(p[:, 3].size), p[:, 3], 1, facecolor='blue',
                      alpha=alpha)
    axr2.fill_between(range(p[:, 7].size), 0.5 - p[:, 7] / 1000, 0.5,
                      facecolor='green', alpha=alpha)
axr2.set_ylabel('Raleigh')
axr2.set_ylim([0, 1])
axr2.set_xlabel('ROF [-] / Restriction Mult. [-] / Transfers [100 MG]')

plt.show()
