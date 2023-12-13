#pragma once
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <utility>

#include "StateValueWrapper.hpp"
#include "StderrWarning.hpp"

namespace HwBackup {

class DescriptorWrapper final: public StateValueWrapper<int> {
 public:
  DescriptorWrapper(int);
  DescriptorWrapper();
  ~DescriptorWrapper();

  DescriptorWrapper(DescriptorWrapper&&) = default;
  DescriptorWrapper& operator=(DescriptorWrapper&&) = default;
};

}  // namespace HwBackup