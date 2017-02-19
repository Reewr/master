#include "Log.hpp"
#include <mmm.hpp>
#include <spdlog/formatter.h>

#include "Utils/Utils.hpp"

unsigned int                    Logging::loggerNameSize = 10;
std::shared_ptr<spdlog::logger> Logging::GLOBAL_LOGGER  = nullptr;

Logging::Formatter::Formatter() {}

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

Logging::Log::Log(const std::string& name) {
  setLoggerName(name);
}

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
