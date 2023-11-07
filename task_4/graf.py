import os;
import datetime;
import matplotlib.pyplot as plt

tm = []

for i in range(1, 25):
    tmp = []
    for j in range(20):
        os.system(f"gcc a.c -DN_THREADS=i -w")
        tic = datetime.datetime.now().timestamp()
        os.system(f"./a.out")
        tac = datetime.datetime.now().timestamp()
        tmp.append(tac - tic)
    tm.append(sum(tmp)/len(tmp))

x = [i for i in range(1, 25)]
plt.plot(x, tm, label = "зависимость времени исполнения от количества потоков");
plt.show()
os.system("rm a.out")