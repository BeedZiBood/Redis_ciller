#include "common.h"
#include <stdexcept>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace common
{
  namespace commands
  {
    std::string put(std::string& line, std::map< std::string, std::string >& store)
    {
      std::string key, value;
      key = get_word(line);
      value = get_word(line);
      store[key] = value;
      BOOST_LOG_TRIVIAL(info) << "PUT key: '" << key << "' value: '" << value << "'";
      return "OK";
    }

    std::string get(std::string& line, std::map< std::string, std::string >& store)
    {
      std::string key;
      key = get_word(line);
      if (store.find(key) == store.end())
      {
        BOOST_LOG_TRIVIAL(info) << "GET key: '" << key << "' not found";
        return "NOT_FOUND";
      }
      else
      {
        BOOST_LOG_TRIVIAL(info) << "GET key: '" << key << "' value: '" << store[key] << "'";
        return store[key];
      }
    }

    std::string del(std::string& line, std::map< std::string, std::string >& store)
    {
      std::string key;
      key = get_word(line);
      if (store.find(key) == store.end())
      {
        BOOST_LOG_TRIVIAL(info) << "DEL key: '" << key << "' not found";
        return "NOT_FOUND";
      }
      else
      {
        store.erase(key);
        BOOST_LOG_TRIVIAL(info) << "DEL key: '" << key << "'";
        return "OK";
      }
    }

    std::string count(std::string& line, std::map< std::string, std::string >& store)
    {
      return std::to_string(store.size());
    }
  }

  Commander createCommander()
  {
    Commander commander;
    commander["PUT"] = commands::put;
    commander["GET"] = commands::get;
    commander["DEL"] = commands::del;
    commander["COUNT"] = commands::count;
    return commander;
  }

  std::string get_word(std::string& line)
  {
    std::string word;
    auto pos = line.find(' ');
    if (pos != std::string::npos && pos > 0)
    {
      word = line.substr(0, pos);
      line.erase(line.begin(), line.begin() + pos + 1);
    }
    else
    {
      if (pos == 0)
      {
        pos = line.find_first_not_of(' ');
        if (pos == std::string::npos)
        {
          throw std::runtime_error("There is not enough arguments");
        }
        line.erase(line.begin(), line.begin() + pos);
        pos = line.find(' ');
        if (pos != std::string::npos)
        {
          word = line.substr(0, pos);
          line.erase(line.begin(), line.begin() + pos + 1);
        }
        else if (line.size() > 0)
        {
          word = line;
          line.clear();
        }
        else
        {
          throw std::runtime_error("There is not enough arguments");
        }
      }
      else
      {
        if (line.size() > 0)
        {
          word = line;
          line.clear();
        }
        else
        {
          throw std::runtime_error("There is not enough arguments");
        }
      }
    }
    for (char ch : word)
    {
      if (!std::isalnum(static_cast< unsigned char >(ch)))
      {
        throw std::runtime_error("Word is not alphanumeric");
      }
    }
    return word;
  }
}
