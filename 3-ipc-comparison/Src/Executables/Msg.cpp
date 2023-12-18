#include "Drivers/MsgDriver.hpp"

int main(int argc, char* argv[]) {
  if (argc != 3)
    PrintMessageAndExit("Expected bufSize and srcFile as arguments");

  size_t bufSize = std::stoul(argv[1]);
  const char* srcFile = argv[2];

  RunMsgDriver(bufSize, srcFile);
}