# Task 5: File transmission via signals
The goal is to transmit a file using Linux signals.

# Implementation

Implementation consists of two executables: one for receiver and one for transmitter. To transmit data, real-time signals are used. Each signal can transmit up to ```sizeof(int)``` bytes. The implementation utilizes three different real-time signals: one for synchronization commands (e.g. acknowledge and etc.), one for ```int``` transmission and one for ```char``` transmission. Both receiver and transmitter use different threads for IO operations and signal handling.

# Conclusion

Even though the signals are not meant to transmit large amounts of data, the task does teach how to work with signals. The implementation transmits files correctly, but does it relatively slow compared to conventional IPC primitives like pipes and etc. The transmission speed is about ```1 Mb/s```.

# How to run

```bash
cd build/5-signal-file-transmission
```

Open two terminals, in first run:

```bash
./RxMain > [OUTPUT_FILE]
```

The program will print its PID.
In second terminal run:

```bash
./TxMain [RX_PID] < [INPUT_FILE]
```

# How to test
There are several end-to-end test which you can run:

```bash
sh GenTestData.sh
python GenTests.py
sh RunTests.sh
```