#ifndef IMPORT_ANIMATION_HPP
#define IMPORT_ANIMATION_HPP

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

#endif