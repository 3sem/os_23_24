#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "DescriptorWrapper.hpp"
#include "HwBackupException.hpp"

namespace HwBackup {
namespace Helpers {
class Pipe {
 private:
  int In;
  int Out;

 public:
  Pipe(int flags = 0) {
    int newPipeFd[2];
    int ret = pipe2(newPipeFd, flags);

    if (ret < 0) THROW_ERRNO("pipe()", errno);

    In = newPipeFd[1];
    Out = newPipeFd[0];
  }

  Pipe(const Pipe&) = delete;
  Pipe& operator=(const Pipe&) = delete;

  Pipe(Pipe&& rhs) {
    In = rhs.In;
    Out = rhs.Out;

    rhs.In = -1;
    rhs.Out = -1;
  };

  Pipe& operator=(Pipe&& rhs) {
    std::swap(In, rhs.In);
    std::swap(Out, rhs.Out);

    return *this;
  }

  ~Pipe() {
    if (In != -1) {
      close(In);
    }

    if (Out != -1) {
      close(Out);
    }
  }

  int GetIn() const { return In; }

  int GetOut() const { return Out; }
};
}  // namespace Helpers
}  // namespace HwBackup