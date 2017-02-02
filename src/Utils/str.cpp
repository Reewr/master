#include "str.hpp"
#include <map>
#include <algorithm>

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
std::string str::toLower(std::string s) {
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
std::string str::toUpper(std::string s) {
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
