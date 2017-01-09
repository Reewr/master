#include "Grid.hpp"

#include <vector>
#include <thread>

#include "../../OpenGLHeaders.hpp"

#include "../../Math/Math.hpp"
#include "../../Utils/Utils.hpp"

GL::Grid::Grid() {
  IBO = 0;
  VBO = 0;
  VAO = 0;
}

GL::Grid::Grid(const vec2& size) {
  IBO = 0;
  VBO = 0;
  VAO = 0;
  change(size);
}

GL::Grid::~Grid() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
  glDeleteVertexArrays(1, &VAO);
}

void GL::Grid::change(const vec2& size) {
  this->size = size;
  setup();
}

void GL::Grid::setup() {
  std::vector<vec4> vertices;
  std::vector<int> indices;
  vertices.reserve(size.x*size.y);
  indices.reserve((size.y-1)*(size.x*2)+(size.y-1));
  //indices.reserve((size.y-1)*(size.x-1)*6);
  generateVertices(vertices);
  generateIndices(indices);
  setupOpenGLArrays(vertices, indices);
}

void GL::Grid::setupOpenGLArrays(const std::vector<vec4> & vertex, const std::vector<int>& ind) {
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*vertex.size(), &vertex[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indicesSize, &ind[0], GL_STATIC_DRAW);
  glBindVertexArray(0);
}

void GL::Grid::generateVertices(std::vector<vec4>& vertices) {
  for(int y = 0; y < size.y; y++) {
    vec2 s = vec2(0, y/(size.y-1));
    for(int x = 0; x < size.x; x++) {
      s.x = x/(size.x-1);
      vertices.push_back(vec4(s-0.5f, s));
    }
  }
}

void GL::Grid::generateTriIndices(std::vector<int>& indices) {
  int quads = (size.x-1)*(size.y-1);
  for(int i = 0; i < quads; i++) {
    int k = i + i / (int)(size.x-1);
    int abcd[4] = {k, k+1, (int)(k+1+size.x), (int)(k+size.x)};
    int t1t2[6] = {abcd[0], abcd[2], abcd[3], abcd[0], abcd[1], abcd[2]};
    indices.insert(indices.end(), t1t2, t1t2+6);
  }
  indicesSize = indices.size();
}

void GL::Grid::generateIndices(std::vector<int>& indices) {
  int n = 0;
  for(int y = 0; y < size.y-1; y++) {
    for(int x = 0; x < size.x*2; x++) {
      int t = x + y * size.x*2;
      if(x == size.x*2 - 1)
        indices.push_back(n);
      else {
        indices.push_back(n);
        if(t % 2 == 0)
          n+= size.x;
        else
          n-= (y%2 == 0) ? (size.x-1) : (size.x+1);
      }
    }
    if(y + 1 < size.y-1)
      indices.push_back(n + size.x);
  }
  indicesSize = indices.size();
}


void GL::Grid::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}