import matplotlib.pyplot as plt
import sys

if (len(sys.argv) != 2):
  print(f"Error: please choose input file [argc={len(sys.argv)}]")
  quit()

file_path = sys.argv[1]
n_points = 0 

x_data = []
y_data = []

with open(file_path, "r") as f:
  lines = f.readlines()
  n_points = int(lines[0])

  for line in lines[1:]:
    n_workers, time = map(float, line.split())
    x_data.append(n_workers)
    y_data.append(time)

plt.xscale("log", base=2)
plt.yscale("log")

plt.plot(x_data, y_data, marker = "x", markeredgecolor="k")

plt.grid()

plt.xlabel("Number of workers")
plt.ylabel("Execution time, s")
plt.title(f"Multi-threaded Monte-Carlo algorithm performance\nTotal number of samples: {n_points}")
plt.savefig("Result.png")  
