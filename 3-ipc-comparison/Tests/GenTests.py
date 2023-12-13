def get_msg_max():
  with open("/proc/sys/kernel/msgmax", "r") as f:
    return int(f.read())
  
BUF_SIZES = [200, 4096, get_msg_max()]
TEST_FILES = ["100b.in", "42b.in", "115b.in", "1mb.in", "4gb.in"]
EXECUTABLES = ["./Fifo", "./Shm", "./Msg"]

tests = []

with open("RunTests.sh", "w") as f:
  for executable in EXECUTABLES:
    f.write(f"echo Testing {executable}...\n")
    for test_file in TEST_FILES:
      f.write("\n")
      for buf_size in BUF_SIZES:
        line = f"{executable} {buf_size} {test_file}"
        f.write(f"echo {line}...\n")
        f.write(f"{line} 2> /dev/null > /dev/null\n" )
        f.write(f"(md5sum {test_file}; md5sum out) | python3 CompareMd5.py\n")
    f.write("\n")