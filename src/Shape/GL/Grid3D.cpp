#include "Grid3D.hpp"

#include <vector>

#include "../../GlobalLog.hpp"
#include "../../Utils/Utils.hpp"

GLGrid3D::GLGrid3D(const mmm::vec2& size) {
  mSize = size;
  setup();
}

GLGrid3D::~GLGrid3D() {}

void GLGrid3D::setup() {
  std::vector<std::vector<mmm::vec3>> vertices(mSize.y,
                                               std::vector<mmm::vec3>(mSize.x));
  std::vector<std::vector<mmm::vec3>> normals(mSize.y,
                                              std::vector<mmm::vec3>(mSize.x));
  std::vector<std::vector<mmm::vec2>> texCoords(mSize.y,
                                                std::vector<mmm::vec2>(
                                                  mSize.x));
  std::vector<int>             indices;
  std::vector<GLShape::Vertex> vertexData;
  vertexData.reserve(mSize.x * mSize.y);
  indices.reserve((mSize.y - 1) * (mSize.x * 2) + (mSize.y - 1));

  generateVertices(vertices, texCoords);
  generateNormals(vertices, normals);
  generateIndices(indices);

  for (unsigned int y = 0; y < vertices.size(); y++) {
    for (unsigned int x = 0; x < vertices[y].size(); x++) {
      vertexData.push_back({ vertices[y][x], texCoords[y][x], normals[y][x] });
    }
  }

  setupOpenGLArrays(vertexData, indices);
}

void GLGrid3D::generateVertices(std::vector<std::vector<mmm::vec3>>& v,
                                std::vector<std::vector<mmm::vec2>>& t) {
  /* Noise::Simplex simp(5, 0.65, 4.0/size.x, 2); */
  for (int y = 0; y < mSize.y - 1; y++) {

    mmm::vec2 s = mmm::vec2(0, y / (mSize.y - 1));

    for (int x = 0; x < mSize.x - 1; x++) {

      s.x       = x / (mSize.x - 1);
      v[y][x]   = mmm::vec3(-0.5 + s.x, 0, -0.5 + s.y);
      t[y][x]   = s * 4;
      v[y][x].y = 0;
    }
  }
}

/**
 * @brief
 *   Generate the normals for the terrain
 *
 *   \todo FIX NORMALS
 *
 * @param v
 * @param n
 */
void GLGrid3D::generateNormals(const std::vector<std::vector<mmm::vec3>>& v,
                               std::vector<std::vector<mmm::vec3>>&       n) {
  std::vector<std::vector<mmm::vec3>> normals1(mSize.y - 1,
                                               std::vector<mmm::vec3>(mSize.x -
                                                                      1));
  std::vector<std::vector<mmm::vec3>> normals2(mSize.y - 1,
                                               std::vector<mmm::vec3>(mSize.x -
                                                                      1));

  auto height = [&v](unsigned int y, unsigned int x) -> float {
    if (y >= v.size())
      return 0.0f;

    if (x >= v[y].size())
      return 0.0f;

    return v[y][x].y;
  };

  for (int y = 0; y < mSize.y - 1; y++) {
    for (int x = 0; x < mSize.x - 1; x++) {
      float hL = height(y - 1, x - 1);
      float hR = height(y + 1, x + 1);
      float hD = height(y - 1, x - 1);
      float hU = height(y + 1, x + 1);

      mmm::vec3 normal = normalize(mmm::vec3(hL - hR, hD - hU, 2.0));
      n[y][x]          = normal;
      // mmm::vec3 triangleNorm0 =
      //   cross(v[y][x] - v[y + 1][x], v[y + 1][x] - v[y + 1][x + 1]);
      // mmm::vec3 triangleNorm1 =
      //   cross(v[y + 1][x + 1] - v[y][x + 1], v[y][x + 1] - v[y][x]);
      // normals1[y][x] = normalize(triangleNorm0);
      // normals2[y][x] = normalize(triangleNorm1);
    }
  }

  // for (int i = 0; i < mSize.y; i++) {
  //   for (int j = 0; j < mSize.x; j++) {
  //     mmm::vec3 finNormal = mmm::vec3(0, 0, 0);

  //     if (j != 0 && i != 0) {
  //       finNormal += normals1[i - 1][j - 1];
  //       finNormal += normals2[i - 1][j - 1];
  //     }

  //     if (i != 0 && j != mSize.x - 1)
  //       finNormal += normals1[i - 1][j];

  //     if (i != mSize.y - 1 && j != mSize.x - 1) {
  //       finNormal += normals1[i][j];
  //       finNormal += normals2[i][j];
  //     }

  //     if (i != mSize.y - 1 && j != 0)
  //       finNormal += normals2[i][j - 1];

  //     n[i][j] = normalize(finNormal);
  //   }
  // }
}

void GLGrid3D::setupOpenGLArrays(const std::vector<GLShape::Vertex>& v,
                                 const std::vector<int>&             i) {
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(GLShape::Vertex) * v.size(),
               &v[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLShape::Vertex), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(GLShape::Vertex),
                        (void*) (sizeof(mmm::vec3)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(GLShape::Vertex),
                        (void*) (sizeof(mmm::vec3) + sizeof(mmm::vec2)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(int) * i.size(),
               &i[0],
               GL_STATIC_DRAW);

  glBindVertexArray(0);
}
