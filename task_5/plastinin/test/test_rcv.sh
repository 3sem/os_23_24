# Generating file
base64 /dev/urandom | head -c 4096 > file.txt

../build/rcv output

md5sum file.txt && md5sum output

rm -rf output
rm -rf file.txt
rm -rf pid
