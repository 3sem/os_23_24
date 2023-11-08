time dd if=/dev/urandom of=total.txt bs=64M count=16
gcc fifo.c -o rnfifo
./rnfifo
md5sum total.txt
md5sum resfifo.txt
rm total.txt
rm resfifo.txt
rm fifo_example
