import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("data.csv")
n_thread = df[df.columns[0]]
time = df[df.columns[1]]

fig, ax = plt.subplots()

ax.set_xlabel('number of thread')
ax.set_ylabel('Elapsed time(in usec)')
ax.plot(n_thread, time)
plt.show()
