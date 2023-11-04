gcc fifo.c -o fifo
gcc shm.c -o shm
gcc mqueue.c -o mqueue
time dd if=/dev/urandom of=total.txt bs=64M count=16
./fifo
./shm
./mqueue
time dd if=/dev/urandom of=total.txt bs=128M count=16
./fifo
./shm
./mqueue
time dd if=/dev/urandom of=total.txt bs=256M count=16
./fifo
./shm
./mqueue
rm total.txt
rm resfifo.txt
rm resshm.txt
rm resqueue.txt
