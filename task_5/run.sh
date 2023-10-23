gcc sigsend.c -o send
gcc sigact.c -o act
./act & ./send
diff first.txt second.txt
md5sum first.txt
md5sum second.txt
rm second.txt
rm pid
