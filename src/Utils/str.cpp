#include "str.hpp"
#include <algorithm>
#include <sstream>
#include <map>

static std::map<unsigned int, std::string> utf8Characters = {};

/**
 * @brief
 *   Turns the string `s` to lowercase, ie. "aAa" becomes
 *   "aaa".
 *
 * @param s
 *   The string to lowercase.
 *
 * @return
 *   Lowercased string
 */
std::string str::toLower(const std::string& st) {
  std::string s = st;
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

/**
 * @brief
 *   Turns the string `s` to uppercase, ie. "aAa" becomes
 *   "AAA".
 *
 * @param s
 *   The string to upper case.
 *
 * @return
 *   Uppercased string
 */
std::string str::toUpper(const std::string& st) {
  std::string s = st;
  std::transform(s.begin(), s.end(), s.begin(), toupper);

  return s;
}

/**
 * @brief
 *   Takes an unsigned integer that represent a unicode character
 *   and turns it into a string. This function uses a map to cache
 *   the answer so it can be run multiple times on the same integer
 *   without the performance hit.
 *
 * @param codepoint
 *
 * @return
 */
std::string str::utf8toStr(unsigned int codepoint) {
  if (utf8Characters.count(codepoint))
    return utf8Characters[codepoint];

  std::string out;

  // this code has been taken from:
  // https://stackoverflow.com/questions/19968705/unsigned-integer-as-utf-8-value
  if (codepoint <= 0x7f)
    out.append(1, static_cast<char>(codepoint));
  else if (codepoint <= 0x7ff) {
    out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  } else if (codepoint <= 0xffff) {
    out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  } else {
    out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  }

  utf8Characters[codepoint] = out;

  return out;
}

/**
 * @brief
 *   Goes throught the given string `s` and replaces every instance of
 *   `replaceStr` with `replaceWith`.
 *
 * @param s
 * @param replaceStr
 * @param replaceWith
 *
 * @return
 */
std::string str::replace(const std::string& s,
                         const std::string& replaceStr,
                         const std::string& replaceWith) {
  size_t pos = s.find(replaceStr);

  // Limit the amount of copying to one if no match
  if (pos == std::string::npos)
    return s;

  std::string retStr = s;

  while (pos != std::string::npos) {
    retStr.replace(pos, replaceStr.size(), replaceWith);

    pos = retStr.find(replaceStr);
  }

  return retStr;
}

/**
 * @brief
 *   Takes two paths and joins them together correctly, handling any double slahes and
 *   double dots.
 *
 * @param path1
 * @param path2
 *
 * @return
 */
std::string str::joinPath(const std::string& path1, const std::string& path2) {
  std::string fullPath = path1 + "/" + path2;

  // Handle ..
  std::vector<std::string> folders = str::split(fullPath, '/');
  std::vector<size_t> dotdots;

  for(size_t i = 0; i < folders.size(); ++i) {
    if (folders[i] == "..") {
      dotdots.push_back(i-1);
      dotdots.push_back(i);
    }
  }

  for(auto& i : dotdots) {
    if (i > 0 && i < folders.size())
      folders[i] = "";
  }

  fullPath = str::join(folders, '/');

  // strip double //
  return str::replace(fullPath, "//", "/");
}

/**
 * @brief
 *   Trims the string from the right, which means any trailing whitespace,
 *   such that "  Hello World!   " becomes "  Hello World!"
 *
 *   Note: the string is sent by reference and does change the contents
 *
 * @param s
 * @returns s
 */
void str::rTrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(),
                       s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace)))
            .base(),
          s.end());
}

/**
 * @brief
 *   Trims the string from the left.
 *   such that "  Hello World!  " becomes "Hello World!  "
 *
 *   Note: the string is sent by reference and does change the contents
 *
 * @param s
 * @returns s
 */
void str::lTrim(std::string& s) {
  s.erase(s.begin(),
          std::find_if(s.begin(),
                       s.end(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))));
}

/**
 * @brief
 *   Trims the string from both left and right
 *   such that "  Hello World!  " becomes "Hello World!"
 *
 *   Note: the string is sent by reference and does change the contents
 *
 * @param s
 */
void str::trim(std::string& s) {
  rTrim(s);
  lTrim(s);
}

/**
 * @brief
 *   Splits the given string on where it can found the given delimiter,
 *   returns a list of strings.
 *
 *   Example:
 *    str::split("1, 2, 3, 4, 5", ',') -> { "1", "2", "3", "4", "5" }
 *
 * @param s
 * @param delimiter
 *
 * @return
 */
std::vector<std::string> str::split(const std::string& s, char delimiter) {
  std::stringstream ss;
  std::string item;
  std::vector<std::string> items;
  ss.str(s);

  while(std::getline(ss, item, delimiter)) {
    items.push_back(item);
  }

  return items;
}

/**
 * @brief
 *  Does the opposite of str::split. Takes a vector of string and joins them
 *  to one string with the delimiter as what seperates them.
 *
 * @param s
 * @param delimiter
 *
 * @return
 */
std::string str::join(const std::vector<std::string>& s, char delimiter) {
  std::string ret = "";
  size_t size = s.size();

  for(unsigned int i = 0; i < size; ++i) {
    ret += s[i];

    if (i + 1 != size) {
      ret += delimiter;
    }
  }

  return ret;
}
