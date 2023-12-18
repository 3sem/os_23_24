import pandas as pd 
import matplotlib.pyplot as plt
import numpy as np

first_exp = pd.read_csv("data100.csv", delimiter=';')
number_of_threads_1 = first_exp[first_exp.columns[0]]
time = (first_exp[first_exp.columns[1]] + first_exp[first_exp.columns[2]] + first_exp[first_exp.columns[3]]) / 3

fig, ax = plt.subplots()

ax.set_xlabel('number of thread')
ax.set_ylabel('Elapsed time(in usec)')
ax.plot(number_of_threads_1, time)
plt.show()
