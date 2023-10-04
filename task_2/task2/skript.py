import os

print("waiting for creating file")
os.system("time dd if=/dev/urandom of=file.txt bs=1048576 count=4096")

print("waiting for program execution")
os.system("time ./a.out")

os.system("diff new_file.txt file.txt")
os.system("md5sum new_file.txt")
os.system("md5sum file.txt")
os.system("ls -l")
os.system("rm file.txt")
os.system("rm new_file.txt")
