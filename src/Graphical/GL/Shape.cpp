#include "Shape.hpp"

GL::Shape::Shape() {}
GL::Shape::~Shape() {
  glDeleteBuffers(1, &IBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}