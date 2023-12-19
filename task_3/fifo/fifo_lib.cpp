#include "fifo.hpp"

Dpipe::Dpipe (size_t _cap) : cap (_cap) {

    buf = (char*) calloc (cap + 1, sizeof (char));
    assert (buf != NULL);

    pipe (fd[0]);
    pipe (fd[1]);
}

size_t Dpipe::send (Dpipe::Directions dir) {

    if (dir != P2C and dir != C2P) {

        perror ("Wrong direction specified");
        return (-1);
    }

    return write (fd[dir][1], buf, size);
}

size_t Dpipe::receive (Dpipe::Directions dir) {

    if (dir != P2C and dir != C2P) {

        perror ("Wrong direction specified");
        return -1;
    }

    size = read (fd[dir][0], buf, cap);

    return size;
}

size_t Dpipe::write2buf (char* src, size_t _size) {

    assert (src != NULL);
    assert (_size >= 0);

    if (_size > cap) return -1;

    size = _size;

    strncpy (buf, src, _size);
    buf[_size] = '\0';

    return size;
}

void Dpipe::toggleBlock (Dpipe::Directions dir) {

    assert (dir == P2C or dir == C2P);

    int fl = fcntl (fd[dir][0], F_GETFL, 0);
    fl ^= O_NONBLOCK;
    fcntl (fd[dir][0], F_SETFL, fl);
}

void createFile (int argc, char* argv[]) {

    bool fl = (argc == 2 and strcmp (argv[1], "--small") == 0);
    char cmd[200];
    sprintf (cmd, "dd if=/dev/urandom of=%s bs=%s count=%s", TEST_FILE_NAME, (fl ? SMALL_FILE_BS : BIG_FILE_BS), (fl ? SMALL_FILE_COUNT : BIG_FILE_COUNT));
    system (cmd);
}

size_t Dpipe::writeBuf2File (FILE* output) {

    assert (output != NULL);

    return fwrite (buf, sizeof (char), size, output);
}

size_t Dpipe::write2bufFromFile (FILE* input, size_t _size) {

    if (_size == 0) _size = cap;

    size = fread (buf, sizeof (char), _size, input);

    return size;
}


void Dpipe::DTOR () {

    free (buf);
    cap = -1;
    size = -1;
    close (fd[0][0]);
    close (fd[0][1]);
    close (fd[1][0]);
    close (fd[0][1]);
}