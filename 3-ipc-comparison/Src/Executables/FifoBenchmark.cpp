#include <iostream>

#include "Drivers/BaseBenchmark.hpp"
#include "Drivers/FifoDriver.hpp"

int main(int argc, char* argv[]) {
  BenchmarkMain(RunFifoDriver, argc, argv);
}