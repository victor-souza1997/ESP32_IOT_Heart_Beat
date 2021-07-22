from scipy.stats import norm
import matplotlib.pyplot as plt
import numpy as np 
import pandas as pd
#initialize a normal distribution with frozen in mean=-1, std. dev.= 1

mean_well = 60; sigma_well = 10;
mean_ill = 120; sigma_ill = 20;

rv = norm(loc = mean_well, scale = sigma_well)
rv1 = norm(loc = mean_ill, scale = sigma_ill)

x = np.arange(0, 200, .1)

#plot the pdfs of these normal distributions 

file = np.array([[mean_well, sigma_well],[mean_ill, sigma_ill]]);
file = pd.DataFrame(file)
file.to_csv (r'../../contents/classificacao.csv', index = False, header= True, encoding='utf-8')
plt.plot(x, rv.pdf(x), x, rv1.pdf(x))
plt.show()
df = pd.read_csv("../../contents/classificacao.csv")
print(df.values)
