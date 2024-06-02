#include "server.h"
#include <functional>
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

Session::Session(tcp::socket socket, std::map< std::string, std::string >& store,
                 std::function< void() > on_disconnect):
  socket_(std::move(socket)),
  commander_(common::createCommander()),
  store_(store),
  on_disconnect_(on_disconnect)
{
}

void Session::start()
{
  BOOST_LOG_TRIVIAL(info) << "Session started";
  do_read();
}

void Session::do_read()
{
  auto self(shared_from_this());
  boost::asio::async_read_until(socket_, buffer_, "\n",
                                [this, self](boost::system::error_code ec, std::size_t length)
                                {
                                  if (!ec)
                                  {
                                    std::istream is(&buffer_);
                                    std::string line;
                                    std::getline(is, line);
                                    if (line.back() == '\r')
                                    {
                                      line.pop_back(); // Remove \r if present
                                    }
                                    std::string response = handle_command(line);
                                    do_write(response);
                                  }
                                  else
                                  {
                                    BOOST_LOG_TRIVIAL(info) << "Client disconnected";
                                    on_disconnect_();
                                  }
                                });
}

void Session::do_write(const std::string& response)
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, boost::asio::buffer(response + "\n"),
                           [this, self](boost::system::error_code ec, std::size_t /*length*/)
                           {
                             if (!ec)
                             {
                               do_read();
                             }
                             else
                             {
                               BOOST_LOG_TRIVIAL(error) << "Error writing: " << ec.message();
                               on_disconnect_();
                             }
                           });
}

std::string Session::handle_command(const std::string& buffer)
{
  try
  {
    std::string line(buffer);
    std::string command = common::get_word(line);

    if (commander_.find(command) != commander_.end())
    {
      auto answer = commander_[command](line, store_);
      return answer;
    }
    else
    {
      BOOST_LOG_TRIVIAL(warning) << "Unknown command: " << command;
      return "ERROR: Unknown command";
    }
  }
  catch (const std::exception& ex)
  {
    BOOST_LOG_TRIVIAL(error) << "Exception handling command: " << ex.what();
    return "ERROR: " + std::string(ex.what());
  }
}


Server::Server(boost::asio::io_context& io_context, short port, size_t max_connections):
  acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
  max_connections_(max_connections),
  current_connections_(0)
{
  BOOST_LOG_TRIVIAL(info) << "Server started on port " << port << " with max connections " << max_connections;
  do_accept();
}

void Server::do_accept()
{
  if (current_connections_ < max_connections_)
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            current_connections_++;
            BOOST_LOG_TRIVIAL(info) << "Accepted new connection. Current connections: " << current_connections_;
            std::make_shared< Session >(std::move(socket), store_,
                                        [this]()
                                        {
                                          current_connections_--;
                                          BOOST_LOG_TRIVIAL(info)
                                              << "Connection closed. Current connections: " << current_connections_;
                                        })
                ->start();
          }
          else
          {
            BOOST_LOG_TRIVIAL(error) << "Accept error: " << ec.message();
          }
          do_accept();
        });
  }
}

void init_logging()
{
  boost::log::add_file_log(
      boost::log::keywords::file_name = "server_%N.log", boost::log::keywords::rotation_size = 10 * 1024 * 1024,
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::format = "[%TimeStamp%] [%Severity%]: %Message%");

  boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%] [%Severity%]: %Message%");

  boost::log::add_common_attributes();
}
