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
#   print f
#   watersources.append(np.loadtxt(f), delimiter=',')

# 0 Volume
# 1 ROF
# 2 Demand
# 3 Demand
# 4 Fund
# 5 Payout
# 6 Price

folder = '' 
# folder = '../HB model results/'

realizations_utilities_all = np.array(p.map(read_append_file, glob.glob(folder + 'Utilities_s0_r*.out')))
policies_all = np.array(p.map(read_append_file, glob.glob(folder + 'Policies_s0_r*.out')))

to_plot = np.random.uniform(-1, 1, len(realizations_utilities_all)) > 0.
realizations_utilities = realizations_utilities_all[to_plot]
policies = policies_all[to_plot]

print "Creating plot"

alpha = 1.0 / len(realizations_utilities_all)   + 0.004
# alpha = 0.005
# print 'alpha=' + str(alpha)


fig, axes = plt.subplots(4, 2, sharex=True)  # , sharey=True)

fig.suptitle('Combined Storage [MG] and ROF (red), Restriction Multiplier (blue) and Transfers (green, up bar means transfer out, and down o/w)')

ax00_2 = axes[0, 0].twinx()
ax00_2.set_ylim([0, 1])
for u in realizations_utilities:
    axes[0, 0].fill_between(range(u[:, 20].size), 0, u[:, 20], facecolor='red', color="none",
                            alpha=alpha)
    ax00_2.fill_between(range(u[:, 23].size), 0, u[:, 23], facecolor='blue', color="none",
                        alpha=alpha)  # ROF
axes[0, 0].set_ylabel('Cary')
axes[0, 0].set_ylim(bottom=0)

ax10_2 = axes[1, 0].twinx()
ax10_2.set_ylim([0, 1])
for u in realizations_utilities:
    axes[1, 0].fill_between(range(u[:, 10].size), 0, u[:, 10], facecolor='red', color="none",
                            alpha=alpha)
    ax10_2.fill_between(range(u[:, 13].size), 0, u[:, 13], facecolor='blue', color="none",
                        alpha=alpha)  # ROF
axes[1, 0].set_ylabel('Durham')
axes[1, 0].set_ylim(bottom=0)

ax20_2 = axes[2, 0].twinx()
ax20_2.set_ylim([0, 1])
for u in realizations_utilities:
    axes[2, 0].fill_between(range(u[:, 0].size), 0, u[:, 0], facecolor='red', color="none",
                            alpha=alpha)
    ax20_2.fill_between(range(u[:, 3].size), 0, u[:, 3], facecolor='blue', color="none",
                        alpha=alpha)  # ROF
axes[2, 0].set_ylabel('OWASA')
axes[2, 0].set_ylim(bottom=0)

ax30_2 = axes[3, 0].twinx()
ax30_2.set_ylim([0, 1])
for u in realizations_utilities:
    axes[3, 0].fill_between(range(u[:, 30].size), 0, u[:, 30], facecolor='red', color="none",
                            alpha=alpha)
    ax30_2.fill_between(range(u[:, 33].size), 0, u[:, 33], facecolor='blue', color="none",
                        alpha=alpha)  # ROF
axes[3, 0].set_ylabel('Raleigh')
axes[3, 0].set_ylim(bottom=0)

axes[3, 0].set_xlabel('Time [weeks]')

# plt.show()

# 0 1 2 3 4 5 6 7
# O D C R           water sources
# O D C R           restrictions
#         O D C R   transfers

# fig2, (axo2, axd2, axc2, axr2) = plt.subplots(4, 1, sharex=True)
# fig2.suptitle('ROF (red), Restriction Multiplier (blue) and Transfers (green)')
for u, p in zip(realizations_utilities, policies):
    axes[0, 1].fill_between(range(u[:, 22].size), 0, u[:, 22], facecolor='red', color="none",
                            alpha=alpha)  # ROF
    axes[0, 1].fill_between(range(p[:, 2].size), p[:, 2], 1, facecolor='blue', color="none",
                      alpha=alpha)  # Restrictions
    axes[0, 1].fill_between(range(p[:, 6].size), 0.5, -p[:, 6] / 1000 + 0.5,
                      facecolor='green', color="none", alpha=alpha)  # Transfers
axes[0, 1].set_ylabel('Cary')
axes[0, 1].set_ylim([0, 1])

for u, p in zip(realizations_utilities, policies):
    axes[1, 1].fill_between(range(u[:, 12].size), 0, u[:, 12], facecolor='red', color="none",
                            alpha=alpha)
    axes[1, 1].fill_between(range(p[:, 1].size), p[:, 1], 1, facecolor='blue', color="none",
                      alpha=alpha)
    axes[1, 1].fill_between(range(p[:, 5].size), 0.5 - p[:, 5] / 1000, 0.5,
                      facecolor='green', color="none", alpha=alpha)
axes[1, 1].set_ylabel('Durham')
axes[1, 1].set_ylim([0, 1])

for u, p in zip(realizations_utilities, policies):
    axes[2, 1].fill_between(range(u[:, 2].size), 0, u[:, 2], facecolor='red', color="none",
                            alpha=alpha)
    axes[2, 1].fill_between(range(p[:, 0].size), p[:, 0], 1, facecolor='blue', color="none",
                      alpha=alpha)
    axes[2, 1].fill_between(range(p[:, 4].size), 0.5 - p[:, 4] / 1000, 0.5,
                      facecolor='green', color="none", alpha=alpha)
axes[2, 1].set_ylabel('OWASA')
axes[2, 1].set_ylim([0, 1])

for u, p in zip(realizations_utilities, policies):
    axes[3, 1].fill_between(range(u[:, 32].size), 0, u[:, 32], facecolor='red', color="none",
                            alpha=alpha)
    axes[3, 1].fill_between(range(p[:, 3].size), p[:, 3], 1, facecolor='blue', color="none",
                      alpha=alpha)
    axes[3, 1].fill_between(range(p[:, 7].size), 0.5 - p[:, 7] / 1000, 0.5,
                      facecolor='green', color="none", alpha=alpha)
axes[3, 1].set_ylabel('Raleigh')
axes[3, 1].set_ylim([0, 1])

axes[3, 1].set_xlabel('ROF [-] / Restriction Mult. [-] / Transfers [BG]')

fig.tight_layout()
print "Showing plot"
plt.show()
