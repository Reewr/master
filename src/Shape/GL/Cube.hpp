#pragma once
#include "Shape.hpp"

#include "../../OpenGLHeaders.hpp"
#include <mmm.hpp>

class GLCube : GLShape {
public:
  GLCube();
  ~GLCube();

  void setup();
  void draw();
};
