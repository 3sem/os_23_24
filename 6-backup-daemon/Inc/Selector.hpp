#pragma once

#include <poll.h>

#include <vector>

#include "Helpers.hpp"

namespace HwBackup {
struct Selector {
 public:
  using IdT = StateValueWrapper<size_t>;

 private:
  std::vector<pollfd> PollFds;

 public:
  IdT Register(int fd, short events);
  void Wait();

  short GetEvents(const IdT& i) const;
};

struct SelectorAlarm {
 private:
  Helpers::Pipe Pipe;
  Selector::IdT SelectorId;

 public:
  SelectorAlarm();

  SelectorAlarm(SelectorAlarm&&) = default;
  SelectorAlarm& operator=(SelectorAlarm&&) = default;

  void RegisterAt(Selector& selector);
  void Alarm();
  bool HadAlarmed(const Selector& selector);
};
}  // namespace HwBackup