#pragma once
#include "Shape.hpp"

#include <mmm.hpp>
#include "../../OpenGLHeaders.hpp"

class GLCube : GLShape {
public:
  GLCube(const mmm::vec2& size);
  ~GLCube();

  void setup();
  void draw();

private:
  mmm::vec2 mSize;
};
