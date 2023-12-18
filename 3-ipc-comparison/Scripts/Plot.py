import matplotlib.pyplot as plt
import sys

if (len(sys.argv) != 2):
  print(f"Error: please choose input file [argc={len(sys.argv)}]")
  quit()

file_path = sys.argv[1]

data = []

with open(file_path, "r") as f:
  lines = f.readlines()
  if (len(lines) % 2) != 0:
    print(f"Error non-even number of lines in file ({len(lines)})")
    quit()

  N = len(lines) // 2
  for i in range(N):
    line1 = lines[2*i].strip().split()
    line2 = lines[2*i + 1].strip().split()

    name = line1[0]
    buf_sizes = list(map(int, line1[1:]))
    times = list(map(float, line2))

    entry = dict()
    entry["name"] = name
    entry["buf_sizes"] = buf_sizes
    entry["times"] = times

    data.append(entry)

fig, ax = plt.subplots()

ax.set_xbound(0, 10**7)
ax.set_ybound(0, 10)

ax.set_xscale("log")
ax.set_yscale("log")

for entry in data:
  ax.plot(entry["buf_sizes"], entry["times"], label=entry["name"], marker = "x", markeredgecolor="k")

ax.vlines([4096], [0], [100], linestyles=['dashed'], label="4Kb", colors=['k'])
ax.vlines([4096 * 16], [0], [100], linestyles=['dashed'], label="64Kb", colors=['k'])

ax.grid()
ax.legend()

ax.set_xlabel("Buffer Size, bytes")
ax.set_ylabel("Execution time, s")
ax.set_title("SYSV IPC comparison, file size 1Gb")
fig.savefig("Result.png")  
