#pragma once

#include <mmm.hpp>
#include <vector>

#include "Shape.hpp"

class GLGrid : public GLShape {
public:
  GLGrid();
  GLGrid(const mmm::vec2& size);
  ~GLGrid();
  void change(const mmm::vec2& size);
  void draw();

  void generateIndices(std::vector<int>& indices);

protected:
  void setup();
  void setupOpenGLArrays(const std::vector<mmm::vec4>& v,
                         const std::vector<int>&       i);
  void generateVertices(std::vector<mmm::vec4>& vertices);
  void generateTriIndices(std::vector<int>& indices);

  mmm::vec2    mSize;
  mmm::vec2    mCellSize;
  int          mCountTriangles;
  unsigned int mIndicesSize;
};