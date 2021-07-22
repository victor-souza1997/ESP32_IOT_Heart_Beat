from scipy.stats import norm
import matplotlib.pyplot as plt
import numpy as np 
import pandas as pd

p1 = pd.read_csv("../../contents/p1.csv")
p2 = pd.read_csv("../../contents/p2.csv")

plt.plot(p1)
plt.plot(p2)
plt.show()