time dd if=/dev/urandom of=total.txt bs=1048576 count=4096
./a.out
diff total.txt resfifo.txt
rm resfifo.txt
rm total.txt
