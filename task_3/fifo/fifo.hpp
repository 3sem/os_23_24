#pragma once
#define NDEBUG 1

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
#include "flog.hpp" //a legacy logging + hash microlibrary by me

#define SMALL_FILE_COUNT "128"
#define SMALL_FILE_BS "128"
#define BIG_FILE_COUNT "5"
#define BIG_FILE_BS "1G"
#define TEST_FILE_NAME "test.rng"
#define OUTPUT_FILE_NAME "test_output.rng"



/// @brief Struct that makes communication from parent to child easy and duplex
struct Dpipe {

    /// @brief enum for direction of send/receive
    enum Directions {

        P2C = 0,
        C2P = 1
    };

    char* buf = NULL; /// buffer pointer
    size_t cap = 0; /// buffer capacity
    size_t size = 0; /// current size of data in buffer
    int fd[2][2] = {0}; /// file descriptors for duplex pipe

    /// @brief A basic constructor that supports custom capacity
    /// @param _cap set capacity of buffer
    Dpipe (size_t _cap);

    /// @brief sends buffer contents through pipe
    /// @param dir direction of transaction
    /// @return same as write()
    size_t send (Directions dir);

    /// @brief recieves and write to buffer from pipe
    /// @param dir direction of reception
    /// @return same as read
    size_t receive (Directions dir);

    size_t write2bufFromFile (FILE* input, size_t _size = 0);

    size_t write2buf (char* src, size_t _size);

    size_t write2buf (char* src);

    void toggleBlock (Directions dir);

    size_t writeBuf2File (FILE* output);

    void DTOR ();
};

void createFile (int argc, char* argv[]);