#pragma once
#include "Grid.hpp"

#include <mmm.hpp>
#include <vector>

class GLGrid3D : public GLGrid {
public:
  GLGrid3D(const mmm::vec2& size);
  ~GLGrid3D();

  void setup();
  void generateVertices(std::vector<std::vector<mmm::vec3>>& vertices,
                        std::vector<std::vector<mmm::vec2>>& texcoords);
  void generateNormals(const std::vector<std::vector<mmm::vec3>>& vertices,
                       std::vector<std::vector<mmm::vec3>>&       normals);

protected:
  void setupOpenGLArrays(const std::vector<Vertex>& v,
                         const std::vector<int>&    i);
};
