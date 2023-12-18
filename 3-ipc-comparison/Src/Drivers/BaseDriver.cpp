#include "Drivers/BaseDriver.hpp"

void PrintErrnoAndExit(const std::string& msg) {
  perror(msg.c_str());
  exit(1);
}

void PrintIPCErrorAndExit(const std::string& msg, IPCStatus status) {
  std::cerr << msg + ": " + std::string{GetErrorDescription(status)} << std::endl;
  exit(1);
}

void PrintMessageAndExit(const std::string& msg) {
  std::cerr << msg << std::endl;
  exit(1);
}