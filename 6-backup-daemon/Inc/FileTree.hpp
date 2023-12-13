#pragma once

#include <dirent.h>

#include <functional>
#include <memory>
#include <stack>
#include <string>

#include "HwBackupException.hpp"
#include "StderrWarning.hpp"

namespace HwBackup {
namespace FileTree {
struct DirWrapper {
  struct DirDeleter {
    void operator()(DIR* dir);
  };

  using DirT = std::unique_ptr<DIR, DirDeleter>;

  static DirT OpenDir(const std::string& path);
};

struct FileWrapper {
  struct FileDeleter {
    void operator()(FILE* file);
  };
  using FileT = std::unique_ptr<FILE, FileDeleter>;

  static FileT OpenFile(const std::string& path, const std::string& mode);
};

using FuncT = std::function<void(const std::string&, const std::string&)>;

void DFS(const std::string& rootPath, FuncT func);

}  // namespace FileTree
}  // namespace HwBackup