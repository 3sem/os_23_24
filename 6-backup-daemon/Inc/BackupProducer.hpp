#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <functional>
#include <stack>
#include <string>
#include <unordered_set>

#include "TnHelpers/FileDescriptor.hpp"
#include "TnHelpers/Exception.hpp"
#include "IEventObserver.hpp"
#include "TnHelpers/Logger.hpp"
#include "PathTree.hpp"

#define HEADER_NAME "HEADER"
#define STAGES_NAME "STAGES"

namespace HwBackup {

class BackupProducer {
 private:
  struct FileDeleter {
    void operator()(FILE* file) { fclose(file); }
  };

  using FileType = std::unique_ptr<FILE, FileDeleter>;

  struct HeaderDataT {
    size_t NStages;
  };

  struct StatResult {
    bool Exists;
    struct stat Stat;
  };

 private:
  TnHelpers::Logger* LoggerPtr;
  std::string DstRoot;
  std::string SrcRoot;

  FileType StagesFile;
  FileType HeaderFile;

  HeaderDataT HeaderData = {};
  IEventObserver::PtrT Observer;

 public:
  BackupProducer(TnHelpers::Logger* loggerPtr, IEventObserver::PtrT&& eventObserver)
      : LoggerPtr{loggerPtr}, Observer{std::move(eventObserver)} {
    if (!loggerPtr) THROW("Logger pointer is null");
  }

  void Open(const std::string& srcPath, const std::string& dstPath, const std::string& cachePrefix) {
    DstRoot = dstPath + cachePrefix;
    SrcRoot = srcPath;

    Observer->Open(SrcRoot, DstRoot);
    PathTree tree;
    tree.AddDir(DstRoot);
    tree.AddDir(SrcRoot);
    Sync(tree);
  }

  static FileType OpenFile(const std::string& filePath) {
    int newFd = open(filePath.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (newFd < 0) THROW_ERRNO("Failed to open fd");

    FILE* newFile = fdopen(newFd, "a+");
    if (!newFile) THROW_ERRNO("Failed open file");

    return {newFile, FileDeleter{}};
  }

  void Sync(PathTree& tree) {
    LOG_INFO(GetLogger(), "Sync forward...");

    tree.VisitPreOrder(
        [this](const std::string& path) { return this->SyncForward(path); });

    LOG_INFO(GetLogger(), "Sync backward...");
    tree.VisitPostOrder(
        [this](const std::string& path) { return this->SyncBackward(path); });
  }

  bool SyncForward(const std::string& path) {
    StatResult src = Stat(SrcRoot + path);
    StatResult dst = Stat(DstRoot + path);

    if (src.Exists == false && dst.Exists) {  // No longer exists
      if (S_ISREG(dst.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Removing file \"" << path << "\"");
        DeleteFile(path);
        return true;
      }
      if (S_ISDIR(dst.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Skipping dir \"" << path << "\"");
        return false;
      }
    }

    if (src.Exists && dst.Exists) {  // File modified
      if (S_ISDIR(src.Stat.st_mode) && S_ISDIR(dst.Stat.st_mode)) return true;

      if (S_ISREG(src.Stat.st_mode) && S_ISREG(dst.Stat.st_mode)) {
        if (!FileWasChanged(src.Stat.st_mtim, dst.Stat.st_mtim)) return true;

        LOG_INFO(GetLogger(), "Modifying file \"" << path << "\"");
        ModifyFile(path);
        return true;
      }

      if (S_ISREG(src.Stat.st_mode) && S_ISDIR(dst.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Skipping dir->file  \"" << path << "\"");
        return false;
      }

      if (S_ISDIR(src.Stat.st_mode) && S_ISREG(src.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Changing file->dir \"" << path << "\"");
        DeleteFile(path);
        CreateDir(path);
        return true;
      }
    }

    if (src.Exists && dst.Exists == false) {  // File created
      if (S_ISDIR(src.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Creating dir \"" << path << "\"");
        CreateDir(path);
        return true;
      }
      if (S_ISREG(src.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Creating file \"" << path << "\"");
        CreateFile(path);
        return true;
      }
    }

    return true;
  }

  bool SyncBackward(const std::string& path) {
    StatResult src = Stat(SrcRoot + path);
    StatResult dst = Stat(DstRoot + path);

    if (src.Exists == false && dst.Exists) {  // No longer exists
      if (S_ISDIR(dst.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Deleting dir \"" << path << "\"");
        DeleteDir(path);
        return true;
      }
    }

    if (src.Exists && dst.Exists) {
      if (S_ISREG(src.Stat.st_mode) && S_ISDIR(dst.Stat.st_mode)) {
        LOG_INFO(GetLogger(), "Changing dir->file \"" << path << "\"");
        DeleteDir(path);
        CreateFile(path);
        return true;
      }
    }

    return true;
  }

  bool FileWasChanged(timespec src, timespec dst) {
    if (src.tv_sec > dst.tv_sec) return true;
    if (src.tv_sec < dst.tv_sec) THROW("Time travel is not possible");
    if (src.tv_sec == dst.tv_sec) return src.tv_nsec > dst.tv_nsec;
    assert(0); // Supress warning
  }

  void CreateDir(const std::string& path) {
    Observer->CreateDir(path);

    std::string dst = DstRoot + path;
    int ret = mkdir(dst.c_str(), 0777);
    if (ret < 0) THROW_ERRNO("mkdir(" + dst + "): ");
  }

  void SyncFile(const std::string& path) {
    std::string dst = DstRoot + path;
    std::string src = SrcRoot + path;

    pid_t pid = fork();

    if (pid < 0) THROW_ERRNO("fork()");

    if (pid == 0) {  // Child
      execlp("cp", "cp", src.c_str(), dst.c_str(), "--preserve", NULL);
    } else {  // Parent
      wait(NULL);
    }
  }

  void CreateFile(const std::string& path) {
    Observer->CreateFile(path);
    SyncFile(path);
  }

  void ModifyFile(const std::string& path) {
    Observer->ModifyFile(path);
    SyncFile(path);
  }

  void DeleteFile(const std::string& path) {
    Observer->DeleteFile(path);

    int ret = unlink((DstRoot + path).c_str());
    if (ret < 0) THROW_ERRNO("unlink()");
  }

  void DeleteDir(const std::string& path) {
    Observer->DeleteDir(path);

    int ret = rmdir((DstRoot + path).c_str());
    if (ret < 0) THROW_ERRNO("rmdir()");
  }

  TnHelpers::Logger& GetLogger() {
    assert(LoggerPtr);
    return *LoggerPtr;
  }

  static StatResult Stat(const std::string& path) {
    struct stat st;
    int ret = stat(path.c_str(), &st);
    if (ret < 0 && errno != ENOENT) THROW_ERRNO("stat(" + path + ")");
    if (ret < 0 && errno == ENOENT) return {false};

    return {true, st};
  }
};  // namespace HwBackup

}  // namespace HwBackup