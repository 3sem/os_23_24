#pragma once

#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include <iostream>

#include "HwBackupException.hpp"
#include "StateValueWrapper.hpp"
#include "StderrWarning.hpp"

namespace HwBackup {
namespace PThread {

class Mutex final : public StateValueWrapper<pthread_mutex_t> {
 public:
  Mutex();
  ~Mutex();

  Mutex(Mutex&&) = default;
  Mutex& operator=(Mutex&&) = default;

  void Lock();
  void Unlock();
};

class Cond final : public StateValueWrapper<pthread_cond_t> {
 public:
  Cond();
  ~Cond();
  void Wait(Mutex& mutex);
  void TimedWait(Mutex& mutex, const timespec& ts);
  void Signal();
};

class Thread final {
 public:
  using Routine = void* (*)(void*);

 private:
  StateValueWrapper<bool> IsJoined;
  StateValueWrapper<pthread_t> Impl;

 public:
  Thread(Routine target, void* args);
  Thread();
  ~Thread();

  Thread(Thread&&) = default;
  Thread& operator=(Thread&&) = default;

 public:
  void Join();
  void Kill(int sig);
  void SetSigmask(int how, const sigset_t& set);
};

}  // namespace PThread
}  // namespace HwBackup