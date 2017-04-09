#include "Grid.hpp"

#include <thread>
#include <vector>

#include "../../OpenGLHeaders.hpp"
#include "../../Utils/Utils.hpp"

GLGrid::GLGrid() {
  IBO = 0;
  VBO = 0;
  VAO = 0;
}

GLGrid::GLGrid(const mmm::vec2& size) {
  IBO = 0;
  VBO = 0;
  VAO = 0;
  change(size);
}

GLGrid::~GLGrid() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
  glDeleteVertexArrays(1, &VAO);
}

void GLGrid::change(const mmm::vec2& size) {
  mSize = size;
  setup();
}

void GLGrid::setup() {
  std::vector<mmm::vec4> vertices;
  std::vector<int>       indices;
  vertices.reserve(mSize.x * mSize.y);
  indices.reserve((mSize.y - 1) * (mSize.x * 2) + (mSize.y - 1));
  // indices.reserve((size.y-1)*(size.x-1)*6);
  generateVertices(vertices);
  generateIndices(indices);
  setupOpenGLArrays(vertices, indices);
}

void GLGrid::setupOpenGLArrays(const std::vector<mmm::vec4>& vertex,
                               const std::vector<int>&       ind) {
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(mmm::vec4) * vertex.size(),
               &vertex[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(int) * mIndicesSize,
               &ind[0],
               GL_STATIC_DRAW);
  glBindVertexArray(0);
}

void GLGrid::generateVertices(std::vector<mmm::vec4>& vertices) {
  for (int y = 0; y < mSize.y; y++) {
    mmm::vec2 s = mmm::vec2(0, y / (mSize.y - 1));
    for (int x = 0; x < mSize.x; x++) {
      s.x = x / (mSize.x - 1);
      vertices.push_back(mmm::vec4(s - 0.5f, s));
    }
  }
}

void GLGrid::generateTriIndices(std::vector<int>& indices) {
  int quads = (mSize.x - 1) * (mSize.y - 1);
  for (int i = 0; i < quads; i++) {
    int k       = i + i / (int) (mSize.x - 1);
    int abcd[4] = { k, k + 1, (int) (k + 1 + mSize.x), (int) (k + mSize.x) };
    int t1t2[6] = { abcd[0], abcd[2], abcd[3], abcd[0], abcd[1], abcd[2] };
    indices.insert(indices.end(), t1t2, t1t2 + 6);
  }

  mIndicesSize = indices.size();
}

void GLGrid::generateIndices(std::vector<int>& indices) {
  int n = 0;
  for (int y = 0; y < mSize.y - 1; y++) {
    for (int x = 0; x < mSize.x * 2; x++) {
      int t = x + y * mSize.x * 2;
      if (x == mSize.x * 2 - 1)
        indices.push_back(n);
      else {
        indices.push_back(n);
        if (t % 2 == 0)
          n += mSize.x;
        else
          n -= (y % 2 == 0) ? (mSize.x - 1) : (mSize.x + 1);
      }
    }

    if (y + 1 < mSize.y - 1)
      indices.push_back(n + mSize.x);
  }

  mIndicesSize = indices.size();
}


void GLGrid::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLE_STRIP, mIndicesSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
