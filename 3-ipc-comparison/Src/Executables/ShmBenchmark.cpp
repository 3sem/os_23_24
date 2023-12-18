#include <iostream>

#include "Drivers/BaseBenchmark.hpp"
#include "Drivers/ShmDriver.hpp"

int main(int argc, char* argv[]) {
  BenchmarkMain(RunShmDriver, argc, argv);
}