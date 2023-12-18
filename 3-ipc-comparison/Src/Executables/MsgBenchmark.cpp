#include <iostream>

#include "Drivers/BaseBenchmark.hpp"
#include "Drivers/MsgDriver.hpp"

int main(int argc, char* argv[]) {
  BenchmarkMain(RunMsgDriver, argc, argv);
}