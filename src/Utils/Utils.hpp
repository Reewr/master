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

bool fileExists(const std::string& name);

bool getGLError();
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
void logTime(std::string loc);

std::string toUpper(std::string s);
std::string toLower(std::string s);

std::string utf8toStr(unsigned int utf8);

void rTrim(std::string& s);
void lTrim(std::string& s);
void trim(std::string& s);

template <typename N>
std::string toStr(N n);

void logPercent(int percent, std::string msg = "");

template <typename V>
void deleteVec(std::vector<V*>& vec);

template <typename V>
void deleteList(std::list<V*>& list);

template <typename K, typename V>
void deleteMap(std::map<K, V*>& map);
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

namespace TEMP {
enum {
  OPTSMENU,
  DROPDOWN,
  SLIDER,
  SLIDERB,
  SPACE,
  TERRAIN,
  TREE,
  BLACK,
  POWERI,
  ROCKI,
  PLANKI,
  COGSI,
  WIREI,
  XMLOPT,
  XMLBUILD,
  XMLRES,
  FONT
};
std::string getPath(int i);
}


#include "Utils.tpp"