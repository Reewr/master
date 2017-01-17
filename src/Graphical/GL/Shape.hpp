#pragma once

#include "../../OpenGLHeaders.hpp"

namespace GL {
  class Shape {
  public:
    Shape();
    virtual ~Shape();
    virtual void setup() = 0;
    virtual void draw()  = 0;

  protected:
    GLuint VBO = 0;
    GLuint IBO = 0;
    GLuint VAO = 0;
  };
}