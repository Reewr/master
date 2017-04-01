#pragma once

#include <string>
#include <vector>

namespace str {
  std::string toUpper(const std::string& s);
  std::string toLower(const std::string& s);

  std::string utf8toStr(unsigned int utf8);

  void rTrim(std::string& s);
  void lTrim(std::string& s);
  void trim(std::string& s);

  std::vector<std::string> split(const std::string& s, char delimiter);
}
