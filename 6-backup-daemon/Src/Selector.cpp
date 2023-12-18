#include "Selector.hpp"

using namespace HwBackup;

auto Selector::Register(int fd, short events) -> IdT {
  pollfd newPollFd;

  newPollFd.fd = fd;
  newPollFd.events = events;

  PollFds.push_back(newPollFd);
  return IdT{PollFds.size() - 1};
}

void Selector::Wait() {
  int ret = poll(PollFds.data(), PollFds.size(), -1);
  if (ret < 0) THROW_ERRNO("poll()", errno);
}

short Selector::GetEvents(const IdT& i) const {
  return PollFds.at(i.Get()).revents;
}

SelectorAlarm::SelectorAlarm() : Pipe{O_NONBLOCK} {}

void SelectorAlarm::RegisterAt(Selector& selector) {
  SelectorId = selector.Register(Pipe.GetOut(), POLLIN);
}

void SelectorAlarm::Alarm() {
  int dummy = 42;
  int ret = write(Pipe.GetIn(), &dummy, 1);
  if (ret < 0) THROW_ERRNO("write()", errno);
}

bool SelectorAlarm::HadAlarmed(const Selector& selector) {
  return selector.GetEvents(SelectorId) & POLLIN;
}