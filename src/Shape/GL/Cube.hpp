#pragma once
#include "Shape.hpp"

#include "../../OpenGLHeaders.hpp"
#include <mmm.hpp>

class GLCube : GLShape {
public:
  GLCube(const mmm::vec2& size);
  ~GLCube();

  void setup();
  void draw();

private:
  mmm::vec2 mSize;
};
