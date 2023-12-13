#pragma once

#include <assert.h>

#include <list>
#include <string>
#include <unordered_map>

#include "TnHelpers/Exception.hpp"
#include "TnHelpers/StderrWarning.hpp"

namespace HwBackup {

/*
 * Watch descriptor cache class
 * Stores wd-path pairs and supports O(1)
 * addition/search/removal
 */
class WDCache final {
 public:
  using WDescriptorT = int;

  struct WDCacheEntry {
    WDescriptorT Wd;
    std::string Path;
  };

  using ListT = std::list<WDCacheEntry>;
  using ListIt = ListT::const_iterator;

 private:
  std::list<WDCacheEntry> List;
  std::unordered_map<WDescriptorT, ListIt> WDMap;
  std::unordered_map<std::string, ListIt> PathMap;

 public:
  WDCache() = default;

  WDCache(const WDCache&) = default;
  WDCache& operator=(const WDCache&) = default;

  WDCache(WDCache&&) = default;
  WDCache& operator=(WDCache&&) = default;
  ~WDCache() = default;

 public:
  void Add(const WDCacheEntry& rhs);
  void Remove(WDescriptorT wd);
  void Remove(const std::string& path);
  void Remove(ListIt iter);

 public:
  ListIt Find(WDescriptorT wd) const;
  ListIt Find(const std::string& path) const;
  ListIt End() const;
  ListIt Begin() const;

  size_t Size() const {
    return List.size();
  }

 private:
  void RemoveImpl(ListIt iter);
};

};  // namespace HwBackup
