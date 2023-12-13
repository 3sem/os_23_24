#include "Logger.hpp"

using namespace HwBackup;

Logger::Logger(StreamPtr&& stream) : StreamImpl{std::move(stream)} {
  if (StreamImpl.Get() == nullptr) THROW("Stream is NULL");
}

std::ostream& Logger::Start(LoggingLevel level) {
  Mutex.Lock();

  assert(StreamImpl.Get());

  PutColor(level);
  PutTimestamp();
  PutLevel(level);
  ResetColor();

  return *StreamImpl.Get();
}

std::ostream& Logger::StartInfo() { return Start(LoggingLevel::Info); }
std::ostream& Logger::StartWarning() { return Start(LoggingLevel::Warn); }
std::ostream& Logger::StartError() { return Start(LoggingLevel::Error); }

void Logger::End() {
  *StreamImpl.Get() << std::endl;
  Mutex.Unlock();
}

void Logger::PutTimestamp() {
  std::string Buf(32, 0);
  time_t now = time(0);

  strftime(&Buf[0], Buf.size(), "%Y-%m-%d %H:%M:%S", localtime(&now));
  *StreamImpl.Get() << "[" << Buf << "]";
}

const char* Logger::LevelToString(LoggingLevel level) {
  using L = LoggingLevel;

  switch (level) {
    case L::Error:
      return "ERROR";
    case L::Warn:
      return "WARNING";
    case L::Info:
      return "INFO";
    default:
      assert(0 && "Unexpected type");
  }
}

void Logger::PutLevel(LoggingLevel level) {
  *StreamImpl.Get() << "[" << LevelToString(level) << "] ";
}

void Logger::PutColor(LoggingLevel level) {
  *StreamImpl.Get() << LevelToColor(level);;
}

void Logger::ResetColor(){
  *StreamImpl.Get() << ResetColorStr();
}

const char* Logger::LevelToColor(LoggingLevel level) {
  using L = LoggingLevel;

  switch (level) {
    case L::Error:
      return "\e[1;31m";
    case L::Warn:
      return "\e[1;33m";
    case L::Info:
      return ResetColorStr();
    default:
      assert(0 && "Unexpected type");
  }
}
const char* Logger::ResetColorStr() { return "\e[0m"; }

Logger Logger::CreateDefault() {
  std::ostream* stream = &std::cerr;
  return Logger{StreamPtr{std::move(stream)}};
}