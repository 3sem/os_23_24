## TASK 5 by Plastinin Alexander

### Build

Make sure you are in `os_23_24/task_6/plastinin`, then run

```
mkdir build && cd build
cmake ..
make
```

### Run

To run a daemon, you need to create a config file.

Config file should look like this:

```
<check_time> <pid> <dump directory>
```

Where
    1. `<check_time>` - Time, in which one backup happens
    2. `pid` - PID of the process, repository of which needed to backup
    3. `<dump directory>` - full path of the backup directory

Program will create `<dump directory>`, then, if the file has been modified or created, it will create backup for it and also save diff between current and previous version in a directory with the same name as file.

### Known problems

    1. If `<pid> directory` contains two files with similar names, program wont work properly.
    2. `<dump directory>` is supposed to has the same structure, like `<pid> directory`, but now it hasn't. Work in progress.
