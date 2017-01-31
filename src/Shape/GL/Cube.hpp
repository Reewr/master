#pragma once

#include "../../OpenGLHeaders.hpp"
#include <mmm.hpp>

#include "Shape.hpp"

class GLCube : GLShape {
public:
  GLCube(const mmm::vec2& size);
  ~GLCube();

  void setup();
  void draw();

private:
  mmm::vec2 mSize;
};
