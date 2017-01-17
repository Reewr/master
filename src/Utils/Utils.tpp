
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
  Utils::logTimeNoEnd();
  if (ENABLE_COLORS && *ENABLE_COLORS) {
    std::cout << "\033[1;31mFatal Error: \033[0m";
  } else {
    std::cout << "Fatal Error: ";
  }
  _utils_detail::fatalError(args...);
}

template <typename... Ts>
void error(Ts... args) {
  if (DEBUG_MODE && *DEBUG_MODE) {
    Utils::logTimeNoEnd();
    if (ENABLE_COLORS && *ENABLE_COLORS) {
      std::cout << "\033[1;31mError: \033[0m";
    } else {
      std::cout << "Error: ";
    }

    _utils_detail::error(args...);
  }
}

template <typename... Ts>
void warning(Ts... args) {
  if (DEBUG_MODE && *DEBUG_MODE) {
    Utils::logTimeNoEnd();
    if (ENABLE_COLORS && *ENABLE_COLORS) {
      std::cout << "\033[1;33mWarning: \033[0m";
    } else {
      std::cout << "Warning: ";
    }
    _utils_detail::warning(args...);
  }
}

template <typename... Ts>
void log(Ts... args) {
  if (DEBUG_MODE && *DEBUG_MODE) {
    Utils::logTimeNoEnd();
    _utils_detail::log(args...);
  }
}

namespace Utils {
  template <typename... Ts>
  void lineLog(Ts... args) {
    if (DEBUG_MODE && *DEBUG_MODE) {
      _utils_detail::lineLog(std::string(250, '\r'));
      Utils::logTimeNoEnd();
      _utils_detail::lineLog(args...);
    }
  }
}

template <typename... Ts>
void tlog(Ts... xs) {
  if (LOOP_LOGGER > 1) {
    Utils::logTimeNoEnd();
    _utils_detail::log(xs...);
  }
}

template <typename N>
std::string Utils::toStr(N n) {
  std::ostringstream buff;
  buff << n;
  return buff.str();
}

template <typename V>
void Utils::deleteVec(std::vector<V*>& vec) {
  for (auto& v : vec)
    delete v;
  vec.clear();
}

template <typename V>
void Utils::deleteList(std::list<V*>& list) {
  for (auto& v : list)
    delete v;
  list.clear();
}

template <typename K, typename V>
void Utils::deleteMap(std::map<K, V*>& map) {
  for (auto& v : map)
    delete v.second;
  map.clear();
}
