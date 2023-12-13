#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "DuplexPipe.h"
#include "stdio.h"

size_t DoPipeSend(Pipe* self, const char* buf, size_t size) {
  assert(self);
  assert(buf);

  size_t nWrite;
  PipeStatus status = PipeSend(self, buf, size, &nWrite);
  if (status == PIPE_SUCCESS) {
    assert(nWrite == size);
    return nWrite;
  }
  fprintf(stderr, "Failed to send: %s\n", GetErrorDescription(status));
  exit(1);
}

size_t DoPipeReceive(Pipe* self, char* buf, size_t size) {
  assert(self);
  assert(buf);

  size_t nRead;
  PipeStatus status = PipeReceive(self, buf, size, &nRead);
  if (status == PIPE_SUCCESS) return nRead;
  fprintf(stderr, "Failed to receive: %s\n", GetErrorDescription(status));
  exit(1);
}

int ReadToBuf(char* buf, size_t size, int* hasEOF) {
  assert(buf);
  assert(hasEOF);

  size_t total = 0;
  while (total != size) {
    int ret = read(STDIN_FILENO, buf + total, size - total);

    if (ret < 0) {
      perror("Failed to read");
      exit(1);
    }

    if (ret == 0) {
      *hasEOF = 1;
      break;
    }

    total += ret;
  }

  return total;
}

int WriteFromBuf(const char* buf, size_t size) {
  assert(buf);

  size_t total = 0;
  while (total != size) {
    int ret = write(STDOUT_FILENO, buf + total, size - total);

    if (ret <= 0) {
      perror("Failed to write");
      exit(1);
    }

    total += ret;
  }

  return total;
}

int Parent(char* buf, size_t bufSize, Pipe* myPipe) {
  PipeSetParentStrategy(myPipe);
  PipeStatus status;

  clock_t begin = clock();

  int readDone = 0;
  int sendDone = 0;

  size_t nSendTotal = 0;

  while (1) {
    if (!sendDone) {
      fprintf(stderr, "Send total: %lu\r", nSendTotal);

      int readSize = ReadToBuf(buf, bufSize, &readDone);
      size_t nWrite = DoPipeSend(myPipe, buf, readSize);

      nSendTotal += nWrite;

      if (readDone) {
        PipeFinish(myPipe);
        sendDone = 1;
      }
    }

    size_t nRead = DoPipeReceive(myPipe, buf, bufSize);
    if (nRead == 0) break;

    WriteFromBuf(buf, nRead);
  }

  PipeDestroy(myPipe);
  fprintf(stderr, "\n");

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  size_t gbytes = nSendTotal / (1 << 30) % (1 << 10);
  size_t mbytes = nSendTotal / (1 << 20) % (1 << 10);
  size_t kbytes = nSendTotal / (1 << 10) % (1 << 10);
  size_t bytes = nSendTotal % (1 << 10);

  fprintf(stderr, "Transferred %lu Gb, %lu Mb, %lu Kb, %lu b\n", gbytes, mbytes,
          kbytes, bytes);
  fprintf(stderr, "Completed in %lf seconds\n", time_spent);
}

int Child(char* buf, size_t bufSize, Pipe* myPipe) {
  PipeSetChildStrategy(myPipe);
  PipeStatus status;

  int done = 0;
  while (1) {
    size_t nRead = DoPipeReceive(myPipe, buf, bufSize);
    size_t nWrite = DoPipeSend(myPipe, buf, nRead);

    if (nRead == 0) {
      PipeFinish(myPipe);
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: argc should be 2");
    exit(1);
  }

  size_t bufSize = strtoul(argv[1], NULL, 10);
  char* buf = (char*)malloc(bufSize);

  if (!buf) {
    fprintf(stderr, "Failed to allocate memory");
    exit(1);
  }

  Pipe myPipe;
  PipeStatus status = PipeInit(&myPipe);

  if (status != PIPE_SUCCESS) {
    fprintf(stderr, "Failed to init pipe: %s\n", GetErrorDescription(status));
    exit(1);
  }

  pid_t pid = fork();

  if (pid < 0) {
    perror("Failed to fork");
    exit(1);
  }
  if (pid > 0) {  // Parent
    Parent(buf, bufSize, &myPipe);
    fprintf(stderr, "Parent finished work, waiting for child's exit...\n");
    wait(NULL);
  }
  if (pid == 0) {  // Child
    Child(buf, bufSize, &myPipe);
  }
}