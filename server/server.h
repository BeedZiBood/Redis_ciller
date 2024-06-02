#ifndef REDIS_KILLER_SERVER_H
#define REDIS_KILLER_SERVER_H
#include <iostream>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "common.h"

using boost::asio::ip::tcp;

class Session: public std::enable_shared_from_this< Session >
{
public:
  Session(tcp::socket socket, std::map< std::string, std::string >& store, std::function< void() > on_disconnect);

  void start();

private:
  void do_read();

  void do_write(const std::string& response);

  std::string handle_command(const std::string& buffer);

  boost::asio::streambuf buffer_;
  tcp::socket socket_;
  std::map< std::string, std::string (*)(std::string&, std::map< std::string, std::string >&) > commander_;
  std::map< std::string, std::string > store_;
  std::function< void() > on_disconnect_;
};

class Server
{
public:
  Server(boost::asio::io_context& io_context, short port, size_t max_connections);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  std::map< std::string, std::string > store_;
  size_t max_connections_;
  size_t current_connections_;
};

void init_logging();
#endif
