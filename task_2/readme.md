# So basically it's a duplex pipe
## Overview
During compiation process this program generates a file with random contents and during runtime "copies" it to a new file. It forks, reads file in child process, sends to parent process, recieves data back and writes to a new file. Afterwards checks integrity and repeats with new buffer size of duplex pipe
## How to run it
To run it in default mode use "make all" and "./duplex.out". To generate smaller file (16 kB instead of 5Gb) use "make small all" and run the same program. "make clean" removes all object files, dependencies files and executables and also .rng random generated files.
