#include "Incremental.hpp"

using namespace HwBackup;
using namespace TnHelpers;

using Incrp = IncrBackupProducer;
using Stage = Incrp::Stage;

bool Incrp::DefaultExitCodeCheck::operator()(int code) { return code == 0; }

const char* Stage::ToString(FileT fileT) {
  switch (fileT) {
    case FileT::Regular:
      return "REG";
    case FileT::Directory:
      return "DIR";
    default:
      assert(0);
  }
}

const char* Stage::ToString(ChangeT chT) {
  switch (chT) {
    case ChangeT::Created:
      return "CREATE";
    case ChangeT::Deleted:
      return "DELETE";
    case ChangeT::Modified:
      return "MODIFY";
    default:
      assert(0);
  }
}

IncrBackupProducer::IncrBackupProducer(const std::string& targetDir,
                                       Logger* loggerPtr)
    : IEventObserver{}, TargetDir{targetDir}, LoggerPtr{loggerPtr} {
  assert(loggerPtr);
}

void Incrp::Open(const std::string& srcRoot, const std::string& dstRoot) {
  SrcRoot = srcRoot;
  DstRoot = dstRoot;

  OpenHeader();
  SyncHeader();
  OpenStages();
}

void Incrp::OpenHeader() {
  HeaderFile = Files::File::Open(GetTarget(HEADER_NAME), "a+");

  size_t nStages;
  int ret = fscanf(HeaderFile.get(), "%zu", &nStages);

  if (ret == EOF && ferror(HeaderFile.get())) THROW_ERRNO("fscanf()");
  if (ret == EOF) nStages = 0;
  if (ret != EOF && ret != 1) THROW("fscanf(): matching error");

  LOG_INFO(GetLogger(), "Opened header, nStages=" << nStages);
  Header.NStages = nStages;
}

void Incrp::OpenStages() {
  StagesFile = Files::File::Open(GetTarget(STAGES_NAME), "a+");
}

void Incrp::SyncHeader() {
  int ret = ftruncate(fileno(HeaderFile.get()), 0);
  if (ret < 0) THROW_ERRNO("ftruncate()");

  ret = fprintf(HeaderFile.get(), "%zu", Header.NStages);
  if (ret < 0) THROW_ERRNO("fprintf()");
  fflush(HeaderFile.get());
}

Logger& Incrp::GetLogger() {
  assert(LoggerPtr);
  return *LoggerPtr;
}

void Incrp::StagesPrint(const std::string& str) {
  int ret = fprintf(StagesFile.get(), "%s", str.c_str());
  fflush(StagesFile.get());
  if (ret < 0) THROW_ERRNO("fprintf()");
}

void Incrp::PutStageRecord(const Stage& stage) {
  std::string TimeBuf(32, 0);
  time_t now = time(0);
  strftime(&TimeBuf[0], TimeBuf.size(), "%Y-%m-%d-%H:%M:%S", localtime(&now));
  StagesPrint(TimeBuf);
  StagesPrint(" "s + Stage::ToString(stage.FileType) + " ");
  StagesPrint(Stage::ToString(stage.ChangeType) + " "s);
  StagesPrint(stage.Path + " ");
  StagesPrint(std::to_string(Header.NStages));

  StagesPrint("\n");

  Header.NStages++;
  SyncHeader();
}

void Incrp::CreateDir(const std::string& path) {
  using FileT = Stage::FileT;
  using ChangeT = Stage::ChangeT;

  Stage stage;
  stage.FileType = FileT::Directory;
  stage.ChangeType = ChangeT::Created;
  stage.Path = path;

  PutStageRecord(stage);
}

void Incrp::CreateFile(const std::string& path) {
  using FileT = Stage::FileT;
  using ChangeT = Stage::ChangeT;

  Stage stage;
  stage.FileType = FileT::Regular;
  stage.ChangeType = ChangeT::Created;
  stage.Path = path;

  std::string fileName = std::to_string(Header.NStages);
  Copy(GetSource(path), GetTarget(fileName));

  PutStageRecord(stage);
}

void Incrp::DeleteDir(const std::string& path) {
  using FileT = Stage::FileT;
  using ChangeT = Stage::ChangeT;

  Stage stage;
  stage.FileType = FileT::Directory;
  stage.ChangeType = ChangeT::Deleted;
  stage.Path = path;

  PutStageRecord(stage);
}

void Incrp::DeleteFile(const std::string& path) {
  using FileT = Stage::FileT;
  using ChangeT = Stage::ChangeT;

  Stage stage;
  stage.FileType = FileT::Regular;
  stage.ChangeType = ChangeT::Deleted;
  stage.Path = path;

  PutStageRecord(stage);
}

void Incrp::ModifyFile(const std::string& path) {
  using FileT = Stage::FileT;
  using ChangeT = Stage::ChangeT;

  Stage stage;
  stage.FileType = FileT::Regular;
  stage.ChangeType = ChangeT::Modified;
  stage.Path = path;

  std::string fileName = std::to_string(Header.NStages);
  MakePatch(GetCached(path), GetSource(path), GetTarget(fileName));

  PutStageRecord(stage);
}

void Incrp::Copy(const std::string& src, const std::string& dst) {
  std::string cmd = "cp " + src + " " + dst;
  System(cmd);
}

void Incrp::MakePatch(const std::string& before, const std::string& after,
                      const std::string& out) {
  std::string cmd = "diff " + before + " " + after + " > " + out;

  // diff exits with 0 if files are equivalent,
  // and with 1 if files are different
  auto check = [](int code) { return code < 2; };

  System(cmd, check);
}

void Incrp::System(const std::string& cmd, std::function<bool(int)> exitCheck) {
  int ret = system(cmd.c_str());
  if (ret < 0) THROW_ERRNO("system()");

  if (WIFEXITED(ret) && !WEXITSTATUS(ret)) return;

  if (!WIFEXITED(ret)) THROW("system(" + cmd + "): exited unexpectedly");

  int code = WEXITSTATUS(ret);
  if (!exitCheck(code))
    THROW("system(" + cmd + "): Exit " + std::to_string(code));
}

std::string Incrp::GetTarget(const std::string& path) const {
  return TargetDir + path;
}

std::unique_ptr<Incrp> Incrp::Create(const std::string& targetDir,
                                     Logger* loggerPtr) {
  return std::make_unique<IncrBackupProducer>(targetDir, loggerPtr);
}
