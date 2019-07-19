import numpy as np
import matplotlib.pyplot as plt

demands_durham_data = np.loadtxt('demands/durham_demand.csv', delimiter=',')
demands_durham_output = np.loadtxt('output/Utilities_s0_r226.csv', delimiter=',', skiprows=1)

plt.plot(demands_durham_data[226, :], lw=3)
plt.plot(demands_durham_output[:, 20])

plt.show()
