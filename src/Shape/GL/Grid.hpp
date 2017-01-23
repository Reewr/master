#pragma once

#include <vector>

#include "../../Math/Math.hpp"
#include "Shape.hpp"

class GLGrid : public GLShape {
public:
  GLGrid();
  GLGrid(const vec2& size);
  ~GLGrid();
  void change(const vec2& size);
  void draw();

  void generateIndices(std::vector<int>& indices);

protected:
  void setup();
  void setupOpenGLArrays(const std::vector<vec4>& v, const std::vector<int>& i);
  void generateVertices(std::vector<vec4>& vertices);
  void generateTriIndices(std::vector<int>& indices);

  vec2         mSize;
  vec2         mCellSize;
  int          mCountTriangles;
  unsigned int mIndicesSize;
};