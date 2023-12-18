#include <iostream>
#include <string>
#include <vector>

template <typename Driver>
std::vector<double> RunBenchmark(Driver driver,
                                 const std::vector<size_t>& bufSizes,
                                 const char* srcFile, size_t nRepeats) {
  std::vector<double> results;

  for (auto bufSize : bufSizes) {
    double totalTime = 0;
    for (int i = 0; i < nRepeats; ++i) totalTime += driver(bufSize, srcFile);
    results.push_back(totalTime / nRepeats);
  }

  return results;
}

template <typename Driver>
void BenchmarkMain(Driver driver, int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "Not enough arguments\nUsage: ./<Name>Benchmark srcFile "
                 "nRepeats bufSize1 [bufSize2] ..."
              << std::endl;
    exit(1);
  }

  const char* srcFile = argv[1];
  size_t nRepeats = std::stoul(argv[2]);

  std::vector<size_t> bufSizes;

  for (int i = 3; i < argc; ++i) bufSizes.push_back(std::stoul(argv[i]));

  std::vector<double> results =
      RunBenchmark(driver, bufSizes, srcFile, nRepeats);

  for (auto result : results) std::cout << result << " ";

  std::cout << std::endl;
}