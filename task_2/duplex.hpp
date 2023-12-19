#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include "flog.hpp" //a legacy logging + hash library by me

#define rngIFname "rng_5gb_file.rng"
#define rngOFname "pipe_output_rng.rng"
#define fileOutputName "results.data"


struct Duplex {

    char* buf = NULL;
    size_t cap = 0;
    size_t size = 0;
    int fd[2][2] = {0}; // fd[0] - parent->child and the other way around

    /// @brief Constructs structure
    /// @param _cap capacity of buffer
    Duplex (size_t _cap) : cap (_cap), size (0) {

        buf = (char*) calloc (cap + 1, sizeof (char));
        assert (buf != NULL);

        pipe (fd[0]);
        pipe (fd[1]);

        fcntl (fd[1][0], F_SETFL, O_NONBLOCK); // Sets parent's read fd to non blocking
    }

    /// @brief Sends contents of buffer in certain direction
    /// @param dir direction of transmission (0 - parent->child)
    /// @return same as write
    size_t send (int dir) {

        if (dir != 0 and dir != 1) {

            perror ("Wrong direction\n");
            exit (-1);
        }

        return write (fd[dir][1], buf, size);
    }

    /// @brief Recieves content from pipe to buffer
    /// @param dir direction of transmission (0 - parent->child)
    /// @return same as read (also new size of data in buffer)
    size_t recieve (int dir) {

        if (dir != 0 and dir != 1) {

            perror ("Wrong direction\n");
            exit (-1);
        }

        size = read (fd[dir][0], buf, cap);

        return size;
    }

    /// @brief basically a wrap for real function
    size_t write2buf (char* src) {

        return write2buf (src, strlen (src));
    }

    /// @brief Basically strcpy of _size chars from src to buffer
    /// @param src source buffer
    /// @param _size amount of bytes to copy
    /// @return amount of bytes written or -1 if doesn't fit
    size_t write2buf (char* src, size_t _size) {

        assert (src != NULL);
        assert (_size >= 0);

        if (_size > cap) return -1;

        size = _size;

        strncpy (buf, src, _size);
        buf[_size] = '\0';
        return size;
    }

    /// @brief Reads _size bytes from file to buffer
    /// @param input src file ptr
    /// @param _size amount of bytes (by default is set to cap using if)
    /// @return size of buffer
    size_t read2bufFromFile (FILE* input, size_t _size = 0) {

        assert (input != NULL);
        assert (_size <= cap);

        if (_size == 0) _size = cap; // here is a default case

        size = fread (buf, sizeof (char), _size, input);

        return size;
    }

    /// @brief Destroy
    void DTOR () {

        free (buf);
        close (fd[0][0]);
        close (fd[0][1]);
        close (fd[1][0]);
        close (fd[1][1]);
        cap = 0;
        size = 0;
    }

};

/// @brief Returns file size by filename
/// @param fileName name of file
/// @return size of file
size_t getFileSize (char* fileName);

/// @brief compares sizes and hashes of input and output
/// @param input input file ptr
/// @param output out file ptr
/// @param result analisys result file ptr
/// @param lol pipe structur (idk why i did this, i could have just created a buffer but still)
void testTransmissionIntegrity (FILE** input, FILE** output, FILE** result, Duplex* lol, clock_t startTime);