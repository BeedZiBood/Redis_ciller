#ifndef REDIS_KILLER_COMMON_H
#define REDIS_KILLER_COMMON_H
#include <functional>
#include <map>
#include <string>

namespace common
{
  using Commander =
      std::map< std::string, std::string (*)(std::string&, std::map< std::string, std::string >&) >;
  std::string get_word(std::string& line);
  Commander createCommander();
}
#endif
