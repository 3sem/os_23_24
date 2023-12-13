# os-sem3-hw
Homeworks for Operating Systems course held in MIPT (Semester 3).

# How to build
```bash
git clone https://github.com/tndrd/os-sem3-hw.git --recursive
cd os-sem3-hw
mkdir build
cd build
cmake ..
cmake --build .
```
> The executables are located in ```/build/<PROJECT_NAME>/``` directories.

# How to sync
```bash
git submodule update --remote
cp -r os-sem3-hw/* .
```
