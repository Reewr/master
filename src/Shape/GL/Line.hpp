#pragma once

#include "Shape.hpp"

class GLLine : GLShape {
public:
  GLLine();
  ~GLLine();

  void setup();
  void draw();

private:
  static int    mLineCounter;
  static GLuint mIBO;
  static GLuint mVBO;
  static GLuint mVAO;
};
