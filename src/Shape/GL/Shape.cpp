#include "Shape.hpp"

GLShape::GLShape() {}
GLShape::~GLShape() {
  glDeleteBuffers(1, &IBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}