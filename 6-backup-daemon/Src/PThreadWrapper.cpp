#include "PThreadWrapper.hpp"

using namespace HwBackup::PThread;

/* ----- Mutex ----- */

Mutex::Mutex() : StateValueWrapper{PTHREAD_MUTEX_INITIALIZER} {}

Mutex::~Mutex() {
  int ret = pthread_mutex_destroy(&Get());
  if (ret != 0) STDERR_WARN_ERRNO("pthread_mutex_destroy()", ret);
}

void Mutex::Lock() {
  int ret = pthread_mutex_lock(&Get());
  if (ret != 0) THROW_ERRNO("pthread_mutex_lock()", ret);
}

void Mutex::Unlock() {
  int ret = pthread_mutex_unlock(&Get());
  if (ret != 0) THROW_ERRNO("pthread_mutex_unlock()", ret);
}

/* ----- Cond ----- */

Cond::Cond() : StateValueWrapper{PTHREAD_COND_INITIALIZER} {}

Cond::~Cond() {
  int ret = pthread_cond_destroy(&Get());
  if (ret != 0) STDERR_WARN_ERRNO("pthread_cond_destroy()", ret);
}

void Cond::Wait(Mutex& mutex) {
  int ret = pthread_cond_wait(&Get(), &mutex.Get());
  if (ret != 0) THROW_ERRNO("pthread_cond_wait()", ret);
}

void Cond::TimedWait(Mutex& mutex, const timespec& ts) {
  int ret = pthread_cond_timedwait(&Get(), &mutex.Get(), &ts);
  if (ret != 0 && ret != ETIMEDOUT)
    THROW_ERRNO("pthread_cond_timedwait()", ret);
}

void Cond::Signal() {
  int ret = pthread_cond_signal(&Get());
  if (ret != 0) THROW_ERRNO("pthread_cond_signal()", ret);
}

/* ----- Thread ----- */

Thread::Thread(Routine target, void* args) : IsJoined{false} {
  int ret = pthread_create(&Impl.Get(), NULL, target, args);
  if (ret != 0) THROW_ERRNO("pthread_create()", ret);
}

Thread::Thread() : IsJoined(true) {}

void Thread::Join() {
  if (IsJoined.Get()) return;

  int ret = pthread_join(Impl.Get(), NULL);
  if (ret != 0) THROW_ERRNO("pthread_join()", ret);

  IsJoined.Get() = true;
}

Thread::~Thread() {
  if (!IsJoined.Get()) STDERR_WARN("Thread is not joined");
}

void Thread::Kill(int sig) {
  if (IsJoined.Get()) THROW("Thread is joined");

  int ret = pthread_kill(Impl.Get(), sig);
  if (ret != 0) THROW_ERRNO("pthread_kill()", ret);
}

void Thread::SetSigmask(int how, const sigset_t& set) {
  int ret = pthread_sigmask(how, &set, NULL);
  if (ret != 0) THROW_ERRNO("pthread_kill()", ret);
}