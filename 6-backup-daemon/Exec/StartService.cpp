#include <fstream>

#include "BackupService.hpp"

using namespace HwBackup;
using namespace TnHelpers;

#define DEFAULT_PERIOD 1000

pid_t CreateDaemon(std::function<int()> routine) {
  pid_t pid = fork();
  if (pid < 0) THROW_ERRNO("fork()");

  if (pid > 0) return pid;

  int ret = routine();
  exit(ret);
}

int BackupServiceMain_Daemon(const std::string& src, const std::string& dst,
                             size_t periodMs) {
  std::ofstream stream{"DaemonLog.txt"};            // Yeah it looks silly
  Logger::StreamPtr streamPtr{std::move(&stream)};  // Need to do smth about it
  Logger logger{std::move(streamPtr)};

  HwBackup::BackupService service{src, dst, periodMs, &logger};

  service.Run();

  sigset_t set;
  int ret;
  sigfillset(&set);
  ret = sigprocmask(SIG_BLOCK, &set, NULL);
  if (ret < 0) {
    LOG_ERROR(logger, "sigprocmask(): "s + strerror(errno));
    return 1;
  }

  sigemptyset(&set);
  sigaddset(&set, SIGINT);

  int sig;
  ret = sigwait(&set, &sig);
  if (ret != 0) {
    LOG_ERROR(logger, "sigwait(): "s + strerror(ret));
    return 1;
  }

  service.Stop();

  LOG_INFO(logger, "Received SIGINT and stopped");
  return 0;
}

int BackupServiceMain_Interactive(const std::string& src,
                                  const std::string& dst, size_t periodMs) {
  Logger logger = Logger::CreateDefault();
  HwBackup::BackupService service{src, dst, periodMs, &logger};

  LOG_INFO(logger, "Press [Enter] to stop...");

  service.Run();
  getchar();
  service.Stop();

  LOG_INFO(logger, "Stopped");
  return 0;
}

struct ServiceArgs {
  std::string Src;
  std::string Dst;
  size_t PeriodMs = DEFAULT_PERIOD;
  bool Daemon = false;
};

ServiceArgs ParseArgs(int argc, char* argv[]) {
  ServiceArgs args;

  if (argc < 3)
    THROW("Usage: ./StartService SRC_DIR DST_DIR [DAEMON] [PERIOD_MS]");

  args.Src = argv[1];
  args.Dst = argv[2];

  if (argc >= 4) args.Daemon = (argv[3] == "DAEMON"s);
  if (argc == 5) {
    args.PeriodMs = std::atol(argv[4]);
    if (args.PeriodMs <= 0) THROW("Bad period value");
  }

  return args;
}

int main(int argc, char* argv[]) {
  auto args = ParseArgs(argc, argv);

  if (args.Daemon) {
    auto routine = [args]() {
      return BackupServiceMain_Daemon(args.Src, args.Dst, args.PeriodMs);
    };

    pid_t pid = CreateDaemon(routine);
    std::cerr << "Daemon created on pid " << pid << std::endl;
    return 0;
  }

  return BackupServiceMain_Interactive(args.Src, args.Dst, args.PeriodMs);
}