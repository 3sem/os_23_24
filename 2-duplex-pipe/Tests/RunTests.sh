echo "Running driver..."
./2-duplex-pipe 100 < 100b.in > 100b.out 2> /dev/null
./2-duplex-pipe 100 < 42b.in > 42b.out 2> /dev/null
./2-duplex-pipe 100 < 115b.in > 115b.out 2> /dev/null
./2-duplex-pipe 10000 < 1mb.in > 1mb.out 2> /dev/null
./2-duplex-pipe 100000 < 4gb.in > 4gb.out 2> /dev/null

echo "Comparing md5..."
(md5sum 100b.in; md5sum 100b.out) | python3 CompareMd5.py 
(md5sum 42b.in; md5sum 42b.out) | python3 CompareMd5.py 
(md5sum 115b.in; md5sum 115b.out) | python3 CompareMd5.py 
(md5sum 1mb.in; md5sum 1mb.out) | python3 CompareMd5.py 
(md5sum 4gb.in; md5sum 4gb.out) | python3 CompareMd5.py 