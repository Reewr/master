#pragma once

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

extern bool* DEBUG_MODE;
extern bool* ENABLE_COLORS;
extern float LOOP_LOGGER;

//! fucking crash already. Seriously stopping program execution should not be
//! this hard. Take a hint and throw a fucking string!
struct Error {
  Error(std::string str) : err(str.c_str()) {}
  Error(const char* errStr) : err(errStr) {}
  const char*       what() const throw() { return err; }

private:
  const char* err;
};

namespace Utils {

  static std::map<unsigned int, std::string> utf8Characters;

  bool getGLError(const std::string& place = "");
  void clearGLError();

  void logTimeStart(std::string name    = "TimeStart",
                    bool        isMS    = true,
                    bool        isPrint = true);
  double logTimeEnd();

  void glLogTimeStart(std::string name    = "glTimeStart",
                      bool        isMS    = true,
                      bool        isPrint = true);
  double glLogTimeEnd();

  void lineLog();
  template <typename... Ts>
  void lineLog(Ts... args);

  void logTimeNoEnd();

  std::string toUpper(std::string s);
  std::string toLower(std::string s);

  std::string utf8toStr(unsigned int utf8);

  void rTrim(std::string& s);
  void lTrim(std::string& s);
  void trim(std::string& s);

  template <typename N>
  std::string toStr(N n);

  void logPercent(int percent, std::string msg = "");
}

//! print fatal error regardless of DEBUG_MODE
void fatalError();
template <typename... Ts>
void fatalError(Ts... args);

//! prints error to standard er if DEBUG_MODE is on
void error();
template <typename... Ts>
void error(Ts... args);

//! prints warning to standard err if DEBUG_MODE is on
void warning();
template <typename... Ts>
void warning(Ts... args);

//! prints to standard out if DEBUG_MODE is on
void log();
template <typename... Ts>
void log(Ts... args);

void tlog();
template <typename... Ts>
void tlog(Ts... args);

#include "Utils.tpp"