#pragma once

#include "XML.hpp"

#include <vector>

namespace Import {

struct Animation : XML {
  int                n = 0;
  int                l = 0;
  std::vector<float> fs;

  Animation(const char* filePath);
};
}