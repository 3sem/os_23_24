TEST_FILES = ["100b.in", "42b.in", "115b.in",
              "4kb.in", "42kb.in", "1mb.in"]
EXECUTABLE = "./RunTest"

with open("RunTests.sh", "w") as f:
    for test_file in TEST_FILES:
        line = f"{EXECUTABLE} < {test_file} > out"
        f.write(f"echo {line}...\n")
        f.write(f"{line} 2> /dev/null\n")
        f.write(
            f"(md5sum {test_file}; md5sum out) | python3 CompareMd5.py\n")
