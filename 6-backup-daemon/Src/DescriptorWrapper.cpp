#include "DescriptorWrapper.hpp"

using namespace HwBackup;

DescriptorWrapper::DescriptorWrapper(int fd)
    : StateValueWrapper{std::move(fd)} {}

DescriptorWrapper::DescriptorWrapper(): StateValueWrapper{-1} {}

DescriptorWrapper::~DescriptorWrapper() {
  if (Get() < 0) return;

  int ret = close(Get());
  if (ret < 0) STDERR_WARN_ERRNO("Close()", errno);
}