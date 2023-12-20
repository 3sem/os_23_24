import os
import signal
import time

os.system("gcc client.c integral/integral.c -o client")
os.system("gcc server.c -o server")
child_pid = os.fork()
if child_pid == 0:
    os.system("./server")
else:
    time.sleep(1)
    os.system("./client")
    os.kill(child_pid, signal.SIGUSR1)
time.sleep(1)
os.system("rm server")
os.system("rm client")