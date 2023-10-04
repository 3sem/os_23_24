time dd if=/dev/urandom of=file.txt bs=1048576 count=4096
./a.out
md5sum file.txt
md5sum total.txt
