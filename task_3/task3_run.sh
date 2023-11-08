gcc fifo.c -o fifo
gcc shm.c -o shm
gcc mqueue.c -o mqueue
time dd if=/dev/urandom of=total.txt bs=64M count=16
./fifo
echo "fifo is ready"
./shm
echo "shm is ready"
./mqueue
killall mqueue
echo "queue is ready"
rm fifo_example
time dd if=/dev/urandom of=total.txt bs=128M count=16
./fifo
echo "fifo is ready 1"
./shm
echo "shm is ready 1"
./mqueue
killall mqueue
echo "queue is ready 1"
time dd if=/dev/urandom of=total.txt bs=256M count=16
./fifo
echo "fifo is ready 2"
./shm
echo "shm is ready 2"
./mqueue
killall mqueue
echo "queue is ready 2"
rm total.txt
rm resfifo.txt
rm resshm.txt
rm resqueue.txt
