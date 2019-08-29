import numpy as np

mdf = np.loadtxt('raleighDemandClassesFractions.csv', delimiter=',')
mucwp = np.loadtxt('raleighUserClassesWaterPrices.csv', delimiter=',')
mstp = np.loadtxt('raleighSewageTreatmentPrice.csv', delimiter=',')

print mdf.shape
print mucwp.shape
print mstp.shape

ave_prices_per_tier = mdf * mucwp
print ave_prices_per_tier.shape

monthly_prices = np.sum(ave_prices_per_tier.T * mstp, axis=0)

print monthly_prices
