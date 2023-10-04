import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df_shm = pd.read_csv("data_shm.csv")
df_queue = pd.read_csv("data_queue.csv")
df_fifo = pd.read_csv("data_fifo.csv")

shm = df_shm['shm']
queue = df_queue['queue']
fifo = df_fifo['fifo']

shm_small = np.average(shm[0:9])* 10**6
shm_medium = np.average(shm[10:19])* 10**6
shm_big = np.average(shm[20:29])* 10**6

fifo_small = np.average(fifo[0:9])* 10**6
fifo_medium = np.average(fifo[10:19])* 10**6
fifo_big = np.average(fifo[20:29])* 10**6

queue_small = np.average(queue[0:9]) * 10**6
queue_medium = np.average(queue[10:19])* 10**6
queue_big = np.average(queue[20:29])* 10**6

print(shm_small, shm_medium, shm_big)
print(fifo_small, fifo_medium, fifo_big)
print(queue_small, queue_medium, queue_big)

fig, ax = plt.subplots()

heights = [shm_small, fifo_small, queue_small, 0, shm_medium, fifo_medium, queue_medium, 0, shm_big, fifo_big, queue_big]
labels = ['1a', '1b', '1c', ' ', '2a', '2b', '2c', ' ', '3a', '3b', '3c']
ax.set_ylabel('Elapsed time(in usec)')
ax.bar(labels, heights)
plt.show()
