#pragma once

#include <sys/wait.h>
#include <unistd.h>

#include "TnHelpers/Exception.hpp"
#include "TnHelpers/Files.hpp"
#include "IEventObserver.hpp"
#include "TnHelpers/Logger.hpp"

#define HEADER_NAME "HEADER"
#define STAGES_NAME "STAGES"

namespace HwBackup {

struct IncrBackupProducer final : public IEventObserver {
 public:
  struct HeaderData {
    size_t NStages = 0;
  };

  using File = TnHelpers::Files::File::Type;

  struct Stage {
    enum class FileT { Regular, Directory };
    enum class ChangeT { Created, Deleted, Modified };

    FileT FileType;
    ChangeT ChangeType;
    std::string Path;

    static const char* ToString(FileT fileT);
    static const char* ToString(ChangeT chT);
  };

  struct DefaultExitCodeCheck {
    bool operator()(int code);
  };

 private:
  std::string TargetDir;
  HeaderData Header;
  File HeaderFile;
  File StagesFile;

  TnHelpers::Logger* LoggerPtr;

 public:
  IncrBackupProducer(const std::string& targetDir, TnHelpers::Logger* loggerPtr);

  virtual void Open(const std::string& srcPath,
                    const std::string& dstPath) override;

  virtual void CreateDir(const std::string& path) override;
  virtual void CreateFile(const std::string& path) override;
  virtual void DeleteDir(const std::string& path) override;
  virtual void DeleteFile(const std::string& path) override;
  virtual void ModifyFile(const std::string& path) override;

  static std::unique_ptr<IncrBackupProducer> Create(
      const std::string& targetDir, TnHelpers::Logger* loggerPtr);

 private:
  void OpenHeader();
  void OpenStages();
  void SyncHeader();

  TnHelpers::Logger& GetLogger();

  void StagesPrint(const std::string& str);

  void PutStageRecord(const Stage& stage);

  void Copy(const std::string& src, const std::string& dst);

  void MakePatch(const std::string& before, const std::string& after,
                 const std::string& out);

  void System(const std::string& cmd,
              std::function<bool(int)> exitCheck = DefaultExitCodeCheck{});

  std::string GetTarget(const std::string& path) const;
};
};  // namespace HwBackup