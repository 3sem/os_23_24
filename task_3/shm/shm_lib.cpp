#include "shm.hpp"

const char SHM_NAME[] = "shmat_of_memory"; // Name for shm
const int CAP = 3; // Capacity of SHM_SIZE array
const size_t SHM_SIZE[CAP] = {128, 4096, 1<<15}; // Shm sizes to test
const long long UPPER_WAIT_LIMIT = 1000000000; // Upper limit of waiting (in iterations)

/// @brief Generates file of predetermined size with random contents
/// @param argc argc from main
/// @param argv argv from main
/// @return size of generated file (-1 if not generated or some error occured with lstat)
/// @note i guess that error of reading a file's size is a sufficient evidence to something going wrong with generation of file (i.e. not generated) or file itself (i.e. bad metadata i guess)
size_t generateFile (int argc, char* argv[]) {

    assert (argc > 0);
    assert (argv != NULL);

    char cmd[100]; //String for command

    bool fl = (argc > 1 and strcmp (argv[1], "--small") == 0); //flag that shows if small file should be generated

    // Command parsing
    sprintf (cmd, "dd if=/dev/urandom of=%s bs=%s count=%s", TEST_FILE_NAME,
            (fl ? SMALL_FILE_BS : BIG_FILE_BS),
                (fl ? SMALL_FILE_COUNT : BIG_FILE_COUNT)); 

    system (cmd); //Execution of command

    struct stat test_file_info;

    // Checking for file existence and not being broken somehow
    if (lstat (TEST_FILE_NAME, &test_file_info) == 0) return test_file_info.st_size;
    else return -1;
}

/// @brief Creates a usable shm segment with SHN_NAME and size parameter
/// @param size size of desired shm segment in bytes
/// @return ptr to beginning of the segment
/// @note If already created in another process than just returns ptr to already existing mem
char* createShm (size_t size) {

    assert (size > 0);

    //Opens a file descriptor to shm with SHM_NAME
    int shmFd = shm_open (SHM_NAME, O_RDWR | O_CREAT, (S_IRWXU | S_IRWXG | S_IRWXO) ^ S_IXUSR ^ S_IXGRP ^ S_IXOTH);

    // Checks for shm_open error
    if (shmFd == -1) {

        perror ("Some error during shm_open");
        return NULL;
    }

    // Resizes to size and checks for errors
    if (ftruncate (shmFd, size) < 0) {

        perror ("Some error during ftruncate");
        return NULL;
    }

    // Maps shmFd to ptr
    void* ptr = mmap (0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    // Checks mmap errors
    if (ptr == NULL) {

        perror ("Some error during mmap");
        return NULL;
    }

    return (char*) ptr;
}

/// @brief Waits till flag has one of set values
/// @param flag ptr to shared mem
/// @param waitTarget what to wait for
/// @note If exceeds number of UPPER_WAIT_LIMIT (~4 or so seconds) iterations then stops the program
void wait4Flag (char* flag, FlagValues waitTarget) {

    // Flag to show if wait limit should be considered (weak point)
    char fl = (*flag) & FL_NO_WAIT_LIMIT;

    // Logs flag values
    flogFlag (flag);

    // Waits until limit reached (exit) or flag value matches (return)
    for (volatile long long i = 0; (fl or i <= UPPER_WAIT_LIMIT) and ((*flag) & (waitTarget | FL_ERROR)) == 0; i++) {

        // logs flag every ↓ this iterations
        if (i % 1000000000ll == 0) flogFlag (flag);

        if (i == UPPER_WAIT_LIMIT and !fl) {

            perror ("Exceeded wait limit, exiting process");
            *flag = FL_ERROR;
            exit (42);
        }
    }
}

/// @brief Logs flag using flog library
/// @param flag ptr to flag
void flogFlag (char* flag) {

    assert (flag != NULL);

    flogprintf ("Flag value :");

    if (*flag == FL_NULL)          flogprintf (" FL_NULL"         );
    if (*flag &  FL_READ)          flogprintf (" FL_READ"         );
    if (*flag &  FL_WROTE)         flogprintf (" FL_WROTE"        );
    if (*flag &  FL_EOF)           flogprintf (" FL_EOF"          );
    if (*flag &  FL_EOF_CONFIRMED) flogprintf (" FL_EOF_CONFIRMED");
    if (*flag &  FL_NO_WAIT_LIMIT) flogprintf (" FL_NO_WAIT_LIMIT");
    if (*flag &  FL_SERVER_READY)  flogprintf (" FL_SERVER_READY" );
    if (*flag &  FL_CLIENT_READY)  flogprintf (" FL_CLIENT_READY" );
    if (*flag &  FL_ERROR)         flogprintf (" FL_ERROR"        );

    flogprintf (";\n");
}