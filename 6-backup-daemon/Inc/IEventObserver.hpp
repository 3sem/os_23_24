#pragma once

#include <string>
#include <memory>

namespace HwBackup {

struct IEventObserver {
 protected:
  std::string SrcRoot;
  std::string DstRoot;

 protected:
  std::string GetSource(const std::string& path) const;
  std::string GetCached(const std::string& path) const;

 public:
  using PtrT = std::unique_ptr<IEventObserver>;

  IEventObserver(const IEventObserver&) = delete;
  IEventObserver& operator=(const IEventObserver&) = delete;

  IEventObserver(IEventObserver&&) = delete;
  IEventObserver& operator=(IEventObserver&&) = delete;

  virtual ~IEventObserver() = default;

 protected:
  IEventObserver() = default;

 public:
  virtual void CreateDir(const std::string& path) = 0;
  virtual void CreateFile(const std::string& path) = 0;
  virtual void DeleteDir(const std::string& path) = 0;
  virtual void DeleteFile(const std::string& path) = 0;
  virtual void ModifyFile(const std::string& path) = 0;
  virtual void Open(const std::string& srcPath, const std::string& dstPath) = 0;

};
}  // namespace HwBackup