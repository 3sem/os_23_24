#include "HwBackupException.hpp"

using namespace HwBackup;

Exception::Exception(const std::string& message, const char* file,
                     const char* function, const char* line)
    : Message{message}, File{file}, Function{function}, Line{line} {}

const char* Exception::what() const noexcept {
  Buf = std::string("HwBackup: file \"");
  (Buf += File) += "\" in function \"";
  (Buf += Function) += "\" on line \"";
  (Buf += Line) += "\": ";
  Buf += Message;

  return Buf.c_str();
}