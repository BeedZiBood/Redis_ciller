#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    std::string line;
    while (std::getline(std::cin, line))
    {
      if (line == "exit")
      {
        break;
      }
      line += "\n";
      boost::asio::write(socket, boost::asio::buffer(line));

      std::string buffer;
      boost::asio::read_until(socket, boost::asio::dynamic_buffer(buffer), '\n');
      std::string response(buffer.data(), buffer.size());
      std::cout << response;
    }
    socket.close();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
