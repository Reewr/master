#pragma once

#include "../../OpenGLHeaders.hpp"
#include <mmm.hpp>

class GLShape {
public:
  struct Vertex {
    mmm::vec3 position;
    mmm::vec2 texCoords;
    mmm::vec3 normals;
  };

  struct ColorVertex {
    mmm::vec3 position;
    mmm::vec4 color;
    mmm::vec3 normals;
  };

  GLShape();
  virtual ~GLShape();
  virtual void setup() = 0;
  virtual void draw()  = 0;

protected:
  GLuint VBO = 0;
  GLuint IBO = 0;
  GLuint VAO = 0;
};
