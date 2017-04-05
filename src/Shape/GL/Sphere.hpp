#pragma once
#include "Shape.hpp"

#include "../../OpenGLHeaders.hpp"
#include <mmm.hpp>

class GLSphere : GLShape {
public:
  GLSphere();
  ~GLSphere();

  void setup();
  void draw();

private:

  static int mSphereCounter;
  static int mNumQuads;
  static GLuint mIBO;
  static GLuint mVBO;
  static GLuint mVAO;
};
