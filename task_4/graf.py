import os;
import datetime;
import matplotlib.pyplot as plt

tm = []

for i in range(1, 50):
    print(i);
    tmp = []
    for j in range(20):
        os.system(f"gcc b.c -DN_THREADS={i} -w -Ipthread.h")
        tic = datetime.datetime.now().timestamp()
        os.system(f"./a.out")
        tac = datetime.datetime.now().timestamp()
        tmp.append(tac - tic)
    tm.append(sum(tmp)/len(tmp))

x = [i for i in range(1, len(tm)+1)]
plt.plot(x, tm, label = "зависимость времени исполнения от количества потоков");
plt.show()
os.system("rm a.out")