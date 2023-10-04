# SCRIPT FOR GENERATING DATA


# Generating small file
base64 /dev/urandom | head -c 400 > file_small.txt

# Generating medium file
base64 /dev/urandom | head -c 4096 > file_medium.txt

# Generating big file
base64 /dev/urandom | head -c 500000 > file_big.txt

for i in $(seq 1 10);
do
    ../build/task_3 --shm file_small.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --queue file_small.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --fifo file_small.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --shm file_medium.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --queue file_medium.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --fifo file_medium.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --shm file_big.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --queue file_big.txt
done

for i in $(seq 1 10);
do
    ../build/task_3 --fifo file_big.txt
done
