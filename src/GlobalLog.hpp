#pragma once

#include "Log.hpp"

template <typename... Args>
void debug(const char* fmt, const Args&... args) {
  Logging::GLOBAL_LOGGER->debug(fmt, args...);
}

template <typename... Args>
void info(const char* fmt, const Args&... args) {
  Logging::GLOBAL_LOGGER->info(fmt, args...);
}

template <typename... Args>
void warn(const char* fmt, const Args&... args) {
  Logging::GLOBAL_LOGGER->warn(fmt, args...);
}

template <typename... Args>
void error(const char* fmt, const Args&... args) {
  Logging::GLOBAL_LOGGER->error(fmt, args...);
}

template <typename... Args>
void critical(const char* fmt, const Args&... args) {
  Logging::GLOBAL_LOGGER->critical(fmt, args...);
}

template <typename T>
void debug(const T& t) {
  Logging::GLOBAL_LOGGER->debug(t);
}

template <typename T>
void info(const T& t) {
  Logging::GLOBAL_LOGGER->info(t);
}

template <typename T>
void warn(const T& t) {
  Logging::GLOBAL_LOGGER->warn(t);
}

template <typename T>
void error(const T& t) {
  Logging::GLOBAL_LOGGER->error(t);
}

template <typename T>
void critical(const T& t) {
  Logging::GLOBAL_LOGGER->critical(t);
}
