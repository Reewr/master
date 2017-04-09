#pragma once
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace Logging {

  /**
   * @brief
   *   This is a class that is a wrapper around an spdLogger. This is to
   *   make it easier to integrate it into existing classes.
   *
   *   All you need to do enable the logger for a class is to inherit from
   *   this class and explicitly call `Log` with the name of your choice.
   *
   *   Should you come into the following case:
   *
   * @example
   *   class A : public Logging::Log {
   *   public:
   *     A() : Logging::Log("A") {}
   *   }
   *
   *   // this cannot be done as A already inherits from Log.
   *   class B : public A, public Logging::Log {
   *   public:
   *     B() : Logging::Log("B") {}
   *   }
   *
   *   // instead, do the following:
   *   class B : public A {
   *   public:
   *     B() {
   *       setLoggerName("B");
   *     }
   *   }
   *
   */
  class Log {
  public:
    //! Creates an instance of Log with a specific name.
    //! It will create a new logger if a logger with the given
    //! name does not already exist
    Log(const std::string& name);

    //! Sets the name of the logger to something different
    //! It will create a new logger if a logger with the given
    //! name does not already exist
    void setLoggerName(const std::string& name);

  protected:
    std::shared_ptr<spdlog::logger> mLog;
  };

  //! The formatter is used to format the output of logging
  class Formatter : public spdlog::formatter {
  public:
    Formatter();
    void format(spdlog::details::log_msg& msg);
  };

  //! The global logger is used in cases (through GlobalLog.hpp)
  //! where you normally don't have a logger
  extern std::shared_ptr<spdlog::logger> GLOBAL_LOGGER;
  extern unsigned int                    loggerNameSize;

  //! initializes the loggers, setting the minimum logging level and
  //! possibly the format. If the format is not specified, it defaults
  //! to using Formatter
  void init(spdlog::level::level_enum level  = spdlog::level::trace,
            const std::string&        format = "");
}
