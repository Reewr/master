#pragma once

#include <iostream>
#include <string>

namespace Utils {
  bool getGLError(const std::string& place = "");
  void clearGLError();

  std::string timeSinceStart();
}
