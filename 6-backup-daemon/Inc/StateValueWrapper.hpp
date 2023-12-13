#pragma once

namespace HwBackup {

template <typename T>
class StateValueWrapper {
 private:
  T Value;

 public:
  explicit StateValueWrapper(T&& value) : Value{std::move(value)} {}
  StateValueWrapper() : Value{} {}

  StateValueWrapper(const StateValueWrapper&) = delete;
  StateValueWrapper& operator=(const StateValueWrapper&) = delete;

  StateValueWrapper(StateValueWrapper&& rhs) {
    Value = std::move(rhs.Value);
    rhs = {};
  }

  StateValueWrapper& operator=(StateValueWrapper&& rhs) {
    std::swap(Value, rhs.Value);
    return *this;
  }

  ~StateValueWrapper() {}

  T& Get() { return Value; }
  const T& Get() const { return Value; }
};

}  // namespace HwBackup