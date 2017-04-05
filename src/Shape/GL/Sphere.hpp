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

  Vertex genVertex(float u, float v);

  static int mSphereCounter;
  static int mNumQuads;
  static GLuint mIBO;
  static GLuint mVBO;
  static GLuint mVAO;
};
