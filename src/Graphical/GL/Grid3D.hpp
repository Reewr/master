#ifndef GRAPHICAL_GL_GRID_3D_HPP
#define GRAPHICAL_GL_GRID_3D_HPP

#include "Grid.hpp"
#include "../../Math/Math.hpp"

#include <vector>

namespace GL {
  class Grid3D : public Grid {
  public:
    struct Vertex {
      vec3 pos;
      vec2 texCoord;
      vec3 norm;
    };

    Grid3D(const vec2& size);
    ~Grid3D();

    void setup(bool skipOpenGL = false);
    void generateVertices(std::vector<std::vector<vec3>>& vertices,
                          std::vector<std::vector<vec2>>& texcoords);
    void generateNormals(const std::vector<std::vector<vec3>>& vertices,
                         std::vector<std::vector<vec3>>& normals);
  protected:
    void setupOpenGLArrays(const std::vector<Vertex>& v, const std::vector<int>& i);
  };
}

#endif