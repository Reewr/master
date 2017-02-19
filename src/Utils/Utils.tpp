
namespace _utils_detail {

  template <typename T>
  void fatalError(T arg) {
    std::cout << arg << std::endl;
  }

  template <typename T, typename... Ts>
  void fatalError(T arg, Ts... args) {
    std::cout << arg;
    fatalError(args...);
  }

  template <typename T>
  void error(T arg) {
    std::cout << arg << std::endl;
  }

  template <typename T, typename... Ts>
  void error(T arg, Ts... args) {
    std::cout << arg;
    error(args...);
  }

  template <typename T>
  void warning(T arg) {
    std::cout << arg << std::endl;
  }

  template <typename T, typename... Ts>
  void warning(T arg, Ts... args) {
    std::cout << arg;
    warning(args...);
  }

  template <typename T>
  void log(T arg) {
    std::cout << arg << std::endl;
  }

  template <typename T, typename... Ts>
  void log(T arg, Ts... args) {
    std::cout << arg;
    log(args...);
  }

  void lineLog();

  template <typename T, typename... Ts>
  void lineLog(T arg, Ts... args) {
    std::cout << arg << std::flush;
    lineLog(args...);
  }

  template <typename T>
  void _tlog(T x) {
    std::cout << x;
  }
}

template <typename... Ts>
void fatalError(Ts... args) {
  std::cout << "Fatal Error: ";
  _utils_detail::fatalError(args...);
}

template <typename... Ts>
void error(Ts... args) {
  std::cout << "Error: ";
  _utils_detail::error(args...);
}

template <typename... Ts>
void warning(Ts... args) {
  std::cout << "Warning: ";
  _utils_detail::warning(args...);
}

template <typename... Ts>
void log(Ts... args) {
  _utils_detail::log(args...);
}

template <typename... Ts>
void tlog(Ts... xs) {
  if (LOOP_LOGGER > 1) {
    _utils_detail::log(xs...);
  }
}
