#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include "server.h"

int main(int argc, char* argv[])
{
  init_logging();

  if (argc != 3)
  {
    BOOST_LOG_TRIVIAL(fatal) << "Usage: server <port> <max_connections>";
    return 1;
  }

  short port = std::atoi(argv[1]);
  size_t max_connections = std::atoi(argv[2]);

  try
  {
    boost::asio::io_context io_context;
    Server server(io_context, port, max_connections);
    io_context.run();
  }
  catch (std::exception& e)
  {
    BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
  }

  return 0;
}
