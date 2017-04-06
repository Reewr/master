#pragma once

#include <string>
#include <vector>

namespace str {
  std::string toUpper(const std::string& s);
  std::string toLower(const std::string& s);

  std::string utf8toStr(unsigned int utf8);

  std::string replace(const std::string& s,
                      const std::string& replaceStr,
                      const std::string& replaceWith);
  std::string joinPath(const std::string& path1,
                       const std::string& path2);

  void rTrim(std::string& s);
  void lTrim(std::string& s);
  void trim(std::string& s);

  std::vector<std::string> split(const std::string& s, char delimiter);
  std::string join(const std::vector<std::string>& s, char delimiter);
}
