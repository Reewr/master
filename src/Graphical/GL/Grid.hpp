#ifndef GRAPHICAL_GL_GRID_HPP
#define GRAPHICAL_GL_GRID_HPP

#include <vector>

#include "Shape.hpp"
#include "../../Math/MathCD.hpp"

namespace GL {

  class Grid : public Shape {
  public:
    Grid();
    Grid(const vec2& size);
    ~Grid();
    void change(const vec2& size);
    void draw();

    void generateIndices(std::vector<int>& indices);
  protected:
    void setup();
    void setupOpenGLArrays(const std::vector<vec4>& v, const std::vector<int>& i);
    void generateVertices(std::vector<vec4>& vertices);
    void generateTriIndices(std::vector<int>& indices);
    vec2 size;
    vec2 cellSize;
    int countTriangles;
    unsigned int indicesSize;
};

}

#endif