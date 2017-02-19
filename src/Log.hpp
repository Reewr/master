#pragma once
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace Logging {
  class Log {
  public:
    Log(const std::string& name);
    void setLoggerName(const std::string& name);

  protected:
    std::shared_ptr<spdlog::logger> mLog;
  };

  class Formatter : public spdlog::formatter {
  public:
    Formatter();
    void format(spdlog::details::log_msg& msg);
  };

  extern std::shared_ptr<spdlog::logger> GLOBAL_LOGGER;
  extern unsigned int                    loggerNameSize;

  void init(spdlog::level::level_enum level  = spdlog::level::trace,
            const std::string&        format = "");
}
