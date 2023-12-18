time dd if=/dev/urandom of=4gb.in bs=1048576 count=4096
time dd if=/dev/urandom of=1mb.in bs=1048576 count=1

time dd if=/dev/urandom of=100b.in bs=1 count=100
time dd if=/dev/urandom of=115b.in bs=1 count=115
time dd if=/dev/urandom of=42b.in bs=1 count=42 