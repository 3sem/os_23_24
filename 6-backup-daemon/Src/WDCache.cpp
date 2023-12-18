#include "WDCache.hpp"

using namespace HwBackup;

/* Base functions */

void WDCache::Add(const WDCacheEntry& rhs) {
  if (Find(rhs.Wd) != End() || Find(rhs.Path) != End())
    THROW("Watch descriptor or path already in cache");

  List.push_front(rhs);
  ListIt iter = List.begin();

  PathMap[rhs.Path] = iter;
  WDMap[rhs.Wd] = iter;
}

void WDCache::RemoveImpl(ListIt iter) {
  if (iter == End()) THROW("Entry not found");

  auto pathIter = Find(iter->Path);
  auto wdIter = Find(iter->Wd);

  if (pathIter != wdIter) THROW("Cache consistency is broken");

  if (pathIter == End()) THROW("Entry is in cache, but its path is not");

  if (wdIter == End()) THROW("Entry is in cache, but its wd is not");

  WDMap.erase(iter->Wd);
  PathMap.erase(iter->Path);
  List.erase(iter);
}

auto WDCache::Find(const std::string& path) const -> ListIt {
  auto pair = PathMap.find(path);
  if (pair == PathMap.end()) return End();
  return pair->second;
}

auto WDCache::Find(WDescriptorT wd) const -> ListIt {
  auto pair = WDMap.find(wd);
  if (pair == WDMap.end()) return End();
  return pair->second;
}

auto WDCache::End() const -> ListIt { return List.cend(); }
auto WDCache::Begin() const -> ListIt { return List.cbegin(); }

void WDCache::Remove(WDescriptorT wd) { return RemoveImpl(Find(wd)); }

void WDCache::Remove(const std::string& path) {
  return RemoveImpl(Find(path));
}

void WDCache::Remove(ListIt iter) { return RemoveImpl(iter); }