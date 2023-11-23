#include "shm.hpp"

const char SHM_NAME[] = "shmat_of_memory";
const size_t SHM_SIZE[] = {128, 4096, 1<<15};
const long long UPPER_WAIT_LIMIT = 1000000000;

/// @brief Generates file of predetermined size with random contents
/// @param argc argc from main
/// @param argv argv from main
/// @return size of generated file (-1 if not generated or some error occured with lstat)
/// @note i guess that error of reading a file's size is a sufficient evidence to something going wrong with generation of file (i.e. not generated) or file itself (i.e. bad metadata i guess)
size_t generateFile (int argc, char* argv[]) {

    assert (argc > 0);
    assert (argv != NULL);

    char cmd[100];

    bool fl = (argc > 1 and strcmp (argv[1], "--small") == 0);

    sprintf (cmd, "dd if=/dev/urandom of=%s bs=%s count=%s", TEST_FILE_NAME,
            (fl ? SMALL_FILE_BS : BIG_FILE_BS),
                (fl ? SMALL_FILE_BS : BIG_FILE_BS));

    system (cmd);

    struct stat test_file_info;

    if (lstat (TEST_FILE_NAME, &test_file_info) == 0) return test_file_info.st_size;
    else return -1;
}

/// @brief Creates a usable shm segment with SHN_NAME and size parameter
/// @param size size of desired shm segment in bytes
/// @return ptr to beginning of the segment
/// @note If already created in another process than just returns ptr to already existing mem
char* createShm (size_t size) {

    assert (size > 0);

    int shmFd = shm_open (SHM_NAME, O_RDWR | O_CREAT, (S_IRWXU | S_IRWXG | S_IRWXO) ^ S_IXUSR ^ S_IXGRP ^ S_IXOTH);

    if (shmFd == -1) {

        perror ("Some error during shm_open");
        return NULL;
    }

    if (ftruncate (shmFd, size) < 0) {

        perror ("Some error during ftruncate");
        return NULL;
    }

    void* ptr = mmap (0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    if (ptr == NULL) {

        perror ("Some error during mmap");
        return NULL;
    }

    return (char*) ptr;
}

/// @brief Waits till flag has set value
/// @param flag ptr to shared mem
/// @param waitTarget what to wait for
/// @note If exceeds number of UPPER_WAIT_LIMIT (~10 seconds) iterations then stops the program
void wait4Flag (char* flag, FlagValues waitTarget) {

    for (volatile int i = 0; i <= UPPER_WAIT_LIMIT and (*flag & waitTarget) == 0; i++) {

        if (i == UPPER_WAIT_LIMIT and *flag != FL_NO_WAIT_LIMIT) {

            perror ("Exceeded wait limit, exiting process");
            *flag = FL_EOF;
            exit (42);
        }
    }
}