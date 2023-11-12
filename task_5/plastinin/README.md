## TASK 5 by Plastinin Alexander

### Build

Make sure you are in `os_23_24/task_5/plastinin`, then run

```
mkdir build && cd build
cmake ..
make
```

### Run

To run an experiment you need to run the sender and the receiver.

First, run the receiver by:
```
./rcv <output file>
```

It will create pid file, needed for sender.

Then, run sender by:
```
./snd <intput file> <pid file>
```

### Test

To do a test, run
```
cd ../test
./test_rcv.sh
```

Then, in another terminal run
```
./test_snd.sh
```

It will create small file, send it through signals and compare resulting files.
