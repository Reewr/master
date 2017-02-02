#pragma once
#include "Grid.hpp"

#include <vector>
#include <mmm.hpp>

class GLGrid3D : public GLGrid {
public:
  struct Vertex {
    mmm::vec3 pos;
    mmm::vec2 texCoord;
    mmm::vec3 norm;
  };

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