#pragma once
#include <type_traits>

#include "BackupProducer.hpp"
#include "FSMonitor.hpp"
#include "FifoChannel.hpp"
#include "Incremental.hpp"
#include "TnHelpers/Logger.hpp"

#define HISTORY_NAME "History/"
#define CHANNEL_NAME "Channel"
#define CACHE_NAME "Cache/"

namespace HwBackup {
class BackupService {
 private:
  std::string SrcPath;
  std::string DstPath;

  TnHelpers::Logger* LoggerPtr;
  TnHelpers::PThread::Thread UpdateThread;
  TnHelpers::PThread::Thread SyncThread;

  mutable TnHelpers::PThread::Mutex Mutex;
  TnHelpers::PThread::Cond Cond;

  FSMonitor Monitor;
  BackupProducer Producer;
  Fifo::Listener Listener;
  PathTree Stages;

  TnHelpers::Selector UpdateSelector;
  TnHelpers::Selector::Alarmer Alarmer;

  size_t Period;

  bool DoStop = false;

 public:
  BackupService(const std::string& srcPath, const std::string& dstPath,
                size_t period, TnHelpers::Logger* loggerPtr)
      : DstPath{ValidatePath(dstPath)},
        SrcPath{ValidatePath(srcPath)},
        Monitor{1, loggerPtr},
        Producer{std::move(ProducerInit(DstPath, loggerPtr))},
        Listener{DstPath + CHANNEL_NAME},
        LoggerPtr{loggerPtr},
        Period(period) {
    Alarmer.RegisterAt(UpdateSelector);
  }

  void Run() {
    DoStop = false;
    Stages.Clear();

    UpdateThread = {UpdateLoopAdapter, this};
    SyncThread = {SyncLoopAdapter, this};

    Listener.Start();

    LOG_INFO(GetLogger(), "Started with period=" << Period);
  }

  void UpdateLoop() {
    Monitor.RegisterAt(UpdateSelector);
    Listener.RegisterAt(UpdateSelector);

    ValidatePath(DstPath + CACHE_NAME);

    Monitor.Start(SrcPath);
    Producer.Open(SrcPath, DstPath, CACHE_NAME);

    while (1) {
      Mutex.Lock();
      if (DoStop) {
        Mutex.Unlock();
        break;
      }
      Mutex.Unlock();

      UpdateSelector.Wait();

      if (Monitor.DataReady(UpdateSelector)) {
        Mutex.Lock();
        Monitor.GetStages(Stages);
        Mutex.Unlock();
      }

      if (Listener.DataReady(UpdateSelector)) {
        size_t newPeriod = Listener.GetData();
        LOG_INFO(GetLogger(), "Setting new period: " << newPeriod);
        SetPeriod(newPeriod);
      }

      if (Alarmer.Triggered(UpdateSelector)) continue;
    }
  }

  static void* UpdateLoopAdapter(void* selfPtr) {
    assert(selfPtr);
    BackupService* self = reinterpret_cast<BackupService*>(selfPtr);
    self->UpdateLoop();
    return NULL;
  }

  void SyncLoop() {
    while (1) {
      Mutex.Lock();

      timespec ts = GetPeriodTimespec();
      Cond.TimedWait(Mutex, ts);

      if (DoStop) {
        Mutex.Unlock();
        break;
      }

      if (!Stages.IsEmpty()) {
        Producer.Sync(Stages);
        Stages.Clear();
      }
      Mutex.Unlock();
    }
  }

  static void* SyncLoopAdapter(void* selfPtr) {
    assert(selfPtr);
    BackupService* self = reinterpret_cast<BackupService*>(selfPtr);
    self->SyncLoop();
    return NULL;
  }

  void Stop() {
    Mutex.Lock();

    DoStop = true;

    Alarmer.Alarm();
    Cond.Signal();
    Mutex.Unlock();

    UpdateThread.Join();
    SyncThread.Join();

    Listener.Stop();
  }

  void SetPeriod(size_t newPeriod) {
    Mutex.Lock();
    Period = newPeriod;
    Mutex.Unlock();
  }

  size_t GetPeriod() const {
    Mutex.Lock();
    size_t period = Period;
    Mutex.Unlock();

    return period;
  }

 private:
  static FSMonitor MonitorInit(TnHelpers::Logger* loggerPtr) {
    return FSMonitor{1, loggerPtr};
  }

  static BackupProducer ProducerInit(const std::string& dstPath,
                                     TnHelpers::Logger* loggerPtr) {
    return BackupProducer{loggerPtr,
                          IncrBackupProducer::Create(
                              ValidatePath(dstPath + HISTORY_NAME), loggerPtr)};
  }

  TnHelpers::Logger& GetLogger() {
    assert(LoggerPtr);
    return *LoggerPtr;
  }

  timespec GetPeriodTimespec() const {
    timespec ts;
    size_t ms = Period;
    clock_gettime(CLOCK_REALTIME, &ts);

    size_t ns_per_s = 1000 * 1000 * 1000;

    size_t ns = (ms % 1000) * 1000 * 1000;
    size_t s = ms / 1000;

    ts.tv_sec += s;
    ts.tv_nsec += ns;

    // Make tv_nsec < 1s
    while (ts.tv_nsec > ns_per_s) {
      ts.tv_nsec -= ns_per_s;
      ts.tv_sec++;
    }

    return ts;
  }

  static std::string ValidatePath(const std::string& path) {
    // Check if it is a dir
    struct stat st;
    int ret = stat(path.c_str(), &st);
    if (ret < 0 && errno != ENOENT) THROW_ERRNO("stat()");

    if (ret < 0 && errno == ENOENT) {
      ret = mkdir(path.c_str(), 0777);
      if (ret < 0) THROW_ERRNO("mkdir()");
    } else if (!S_ISDIR(st.st_mode))
      THROW("Error: " + path + " is not a directory");

    if (path.back() != '/') return path + "/";

    return path;
  }
};

}  // namespace HwBackup
