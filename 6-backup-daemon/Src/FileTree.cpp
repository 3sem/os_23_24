#include "FileTree.hpp"

using namespace HwBackup;
using namespace FileTree;

void DirWrapper::DirDeleter::operator()(DIR* dir) { closedir(dir); };

auto DirWrapper::OpenDir(const std::string& path) -> DirT {
  DirT newDir = {opendir(path.c_str()), DirDeleter{}};
  if (!newDir.get())
    THROW_ERRNO("Failed to open directory \"" + path + "\"", errno);
  return newDir;
}

void FileWrapper::FileDeleter::operator()(FILE* file) { fclose(file); };

auto FileWrapper::OpenFile(const std::string& path, const std::string& mode)
    -> FileT {
  FileT newFile = {fopen(path.c_str(), mode.c_str()), FileDeleter{}};
  if (!newFile.get())
    THROW_ERRNO("Failed to open file \"" + path + "\"", errno);
  return newFile;
}

void FileTree::DFS(const std::string& rootPath, FuncT func) {
  std::stack<std::string> stk;
  stk.push(".");

  func(".", rootPath);

  while (!stk.empty()) {
    std::string nodePath = std::move(stk.top());
    stk.pop();

    std::string path = rootPath + nodePath;

    DirWrapper::DirT dir = DirWrapper::OpenDir(path);
    struct dirent* de;
    while ((de = readdir(dir.get())) != NULL) {
      if (strcmp(".", de->d_name) == 0 || strcmp("..", de->d_name) == 0)
        continue;

      std::string pathName = nodePath + "/" + de->d_name;

      func(nodePath, de->d_name);
      if (de->d_type == DT_DIR) stk.push(pathName);
    }
  }
}