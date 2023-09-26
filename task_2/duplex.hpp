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
#include "flog.hpp"

#define rngIFname "rng_5gb_file.rng"
#define rngOFname "pipe_output_rng.rng"
#define fileOutputName "results.data"

size_t getFileSize (char* fileName);

struct Duplex {

    char* buf = NULL;
    size_t cap = 0;
    size_t size = 0;
    int fd[2][2] = {0}; // fd[0] - parent->child and the other way around

    Duplex (size_t _cap) : cap (_cap), size (0) {

        buf = (char*) calloc (cap + 1, sizeof (char));
        assert (buf != NULL);

        pipe (fd[0]);
        pipe (fd[1]);

        fcntl (fd[1][0], F_SETFL, O_NONBLOCK);
    }

    /// @brief Sends contents of buffer in certain direction
    /// @param dir direction of transmission (0 - parent->child)
    /// @return something i haven't thought about yet
    size_t send (int dir) {

        if (dir != 0 and dir != 1) {

            perror ("Wrong direction\n");
            exit (-1);
        }

        return write (fd[dir][1], buf, size);
    }

    size_t recieve (int dir) {

        if (dir != 0 and dir != 1) {

            perror ("Wrong direction\n");
            exit (-1);
        }

        size = read (fd[dir][0], buf, cap);

        // if (size == -1) {

        //     perror ("Some read error\n");
        //     exit (-1);
        // }

        return size;
    }

    size_t write2buf (char* src) {

        return write2buf (src, strlen (src));
    }

    size_t write2buf (char* src, size_t _size) {

        assert (src != NULL);
        assert (_size >= 0);

        if (_size > cap) return -1;

        size = _size;

        strncpy (buf, src, _size);
        buf[_size] = '\0';
    }

    size_t read2bufFromFile (FILE* input, size_t _size = 0) {

        assert (input != NULL);
        assert (_size <= cap);

        if (_size == 0) _size = cap;

        size = fread (buf, sizeof (char), _size, input);

        return size;
    }

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

void testTransmissionIntegrity (FILE** input, FILE** output, FILE** result, Duplex* lol);