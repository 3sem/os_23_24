# Task 2: Full-duplex pipe
The goal is to create an IPC data structure that supports simultaneous reading and writing.
The implementation is based on two regular Linux pipes that provide forward and backward data flows respectively.
The implemented structure involves Strategy design pattern: read() and write() methods are set different for the structure enities in parent/child processes.

# Main driver behaviour
The main executable reads data from ```stdin```, sends it to the child, which forwards the data back to parent. Then the parent prints received data to ```stdout```.

# How to run
```bash
cd build/2-duplex-pipe
./2-duplex-pipe < data.in > data.out
```

The output data should be equal to input data. The result can be verified via checksum comparison:

```bash
md5sum data.in && md5sum data.out
```

# How to test
There are several automated tests which you can run:
> Warning: ```GetTestData.sh``` generates several files with one of them being **4Gb** of size.

```bash
sh GenTestData.sh
sh RunTests.sh
```