#include "IEventObserver.hpp"

using namespace HwBackup;

std::string IEventObserver::GetSource(const std::string& path) const {
  return SrcRoot + path;
}

std::string IEventObserver::GetCached(const std::string& path) const {
  return DstRoot + path;
}