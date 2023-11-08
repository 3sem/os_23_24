time dd if=/dev/urandom of=total.txt bs=256M count=16 
time gcc shm.c
./shm
md5sum total.txt
md5sum resshm.txt
rm total.txt
rm resshm.txt
