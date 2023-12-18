import os
import time
pid = int(input())

os.system(f"./snd input.txt {pid}")

time.sleep(1)

os.system("rm rcv")
os.system("rm snd")