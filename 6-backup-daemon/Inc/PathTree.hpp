#pragma once

#include <assert.h>
#include <dirent.h>

#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

#include "TnHelpers/Exception.hpp"
#include "TnHelpers/StderrWarning.hpp"
#include "TnHelpers/Files.hpp"

namespace HwBackup {

struct PathTree final {
 private:
  struct Node final {
    using PtrT = std::unique_ptr<Node>;

    std::string Path;
    std::vector<PtrT> Children;
    bool VisitorDone = false;

    Node(const std::string& path);
  };

 private:
  Node::PtrT Root;
  std::unordered_map<std::string, Node*> Map;

 public:
  using VisitF = std::function<bool(const std::string&)>;

  PathTree();

  void AddPath(const std::string& path, const std::string& name);
  void AddDir(const std::string& dirPath);

  void VisitPostOrder(VisitF func);
  void VisitPreOrder(VisitF func);
  bool IsEmpty() const;
  void Clear();

  PathTree(const PathTree&) = delete;
  PathTree& operator=(const PathTree&) = delete;

  PathTree(PathTree&&) = default;
  PathTree& operator=(PathTree&&) = default;

  ~PathTree() = default;

  void Dump(std::ostream& os) const;

 private:
  void VisitRecursivePostOrder(Node& node, VisitF func);
  void VisitRecursivePreOrder(Node& node, VisitF func);
};

}  // namespace HwBackup