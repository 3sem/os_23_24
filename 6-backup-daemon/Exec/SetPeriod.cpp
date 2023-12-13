#include "FifoChannel.hpp"

using namespace HwBackup;

int main(int argc, char* argv[]) {
  if (argc != 3) THROW("Usage: ./SetPeriod CHANNEL_PATH PERIOD_MS");

  long value = std::atol(argv[2]);
  if (value <= 0) THROW("Bad value");

  std::string path = argv[1];
  size_t period = value;

  Fifo::Client client;

  client.ConnectTo(path);
  client.Send(period);
  client.Disconnect();
}