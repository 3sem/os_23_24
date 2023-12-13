msgMax=$( cat /proc/sys/kernel/msgmax )

srcFile=1gb.in
nRepeats=5

bufSizesFifo="16 128 4096 65536 1048576 16777216"
bufSizesShm="16 128 4096 65536 1048576 16777216"
bufSizesMsg="16 128 4096 $msgMax"

echo "Fifo ${bufSizesFifo}"
./FifoBenchmark $srcFile $nRepeats $bufSizesFifo 2> /dev/null

echo "Shm ${bufSizesShm}"
./ShmBenchmark $srcFile $nRepeats $bufSizesShm 2> /dev/null

echo "Msg ${bufSizesMsg}"
./MsgBenchmark $srcFile $nRepeats $bufSizesMsg 2> /dev/null
