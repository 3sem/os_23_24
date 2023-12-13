#include "BackupService.hpp"

using namespace HwBackup;

int main(int argc, char* argv[]) {
  if (argc != 3)
    std::cerr << "Usage: ./BackupTest SRC_PATH DST_PATH" << std::endl;

  std::string src = argv[1];
  std::string dst = argv[2];

  auto logger = TnHelpers::Logger::CreateDefault();
  HwBackup::BackupService service {src, dst, 10000, &logger};

  service.Run();
  getchar();
  service.Stop();

  LOG_INFO(logger, "Stopped");
}