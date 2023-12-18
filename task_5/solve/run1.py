import os

os.system("gcc sigrcv.c -o rcv")
os.system("gcc sigsendl.c -o snd")

os.system("./rcv output.txt")



