#include "TxDriver.h"

int main(int argc, char* argv[]) {
  if (argc != 2) ExitWithMessage("Wrong arguments, expected <RX_PID>\n");

  pid_t rxPid = atoi(argv[1]);

  TxDriver(rxPid);
}