#include "Log.hpp"
#include <mmm.hpp>
#include <spdlog/formatter.h>

#include "Utils/Utils.hpp"

//! Sets the maximum length of a logger name. When things are logged,
//! the names of the logger gets padded to a certain length, this is
//! that length.
unsigned int Logging::loggerNameSize = 10;

//! Since not all functions that needs to log are within a class that
//! can inherit from the logger, there also exists a global logger
//! stored here.
std::shared_ptr<spdlog::logger> Logging::GLOBAL_LOGGER = nullptr;

Logging::Formatter::Formatter() {}

/**
 * @brief
 *   Formats the log entry to the following format:
 *
 *   [timeSinceStart mm:ss.ms] [level] [name] loginfo
 *
 * @param msg
 */
void Logging::Formatter::format(spdlog::details::log_msg& msg) {
  std::string level = spdlog::level::to_str(msg.level);
  std::string name  = *msg.logger_name;

  msg.formatted << "[" << Utils::timeSinceStart() << "] ";
  msg.formatted << "[" << level << "] ";

  if (level.size() < 5)
    msg.formatted << std::string(5 - level.size(), ' ');

  msg.formatted << "[" << name << "] ";

  if (name.size() < Logging::loggerNameSize)
    msg.formatted << std::string(Logging::loggerNameSize - name.size(), ' ');

  msg.formatted << fmt::StringRef(msg.raw.data(), msg.raw.size());

  msg.formatted.write(spdlog::details::os::eol, spdlog::details::os::eol_size);
}

/**
 * @brief
 *   Creates a Logger of a specific name, using an instance of a
 *   logger if it already exists or creating a new one.
 *
 * @param name
 */
Logging::Log::Log(const std::string& name) {
  setLoggerName(name);
}

/**
 * @brief
 *   Sets the name of the logger, effectively creating a new logger
 *   if the name does not already exist.
 *
 * @param name
 */
void Logging::Log::setLoggerName(const std::string& name) {
  std::shared_ptr<spdlog::logger> logger = spdlog::get(name);
  mLog = !logger ? spdlog::stdout_color_mt(name) : logger;
}


void Logging::init(spdlog::level::level_enum level, const std::string& format) {
  if (format != "") {
    spdlog::set_pattern(format);
  } else {
    spdlog::set_formatter(std::make_shared<Formatter>());
  }
  spdlog::set_level(level);

  GLOBAL_LOGGER = spdlog::stdout_color_mt("Global");
}
