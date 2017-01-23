#include "Grid3D.hpp"

#include <vector>

#include "../../Math/Math.hpp"

template <typename T>
using dVector = std::vector<std::vector<T>>;

template <typename T>
using vector = std::vector<T>;

GLGrid3D::GLGrid3D(const vec2& size) {
  mSize = size;
  // this->change(size);
}

GLGrid3D::~GLGrid3D() {}

void GLGrid3D::setup() {
  dVector<vec3>  vertices(mSize.y, vector<vec3>(mSize.x));
  dVector<vec3>  normals(mSize.y, vector<vec3>(mSize.x));
  dVector<vec2>  texCoords(mSize.y, vector<vec2>(mSize.x));
  vector<int>    indices;
  vector<Vertex> vertexData;
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

void GLGrid3D::generateVertices(dVector<vec3>& v, dVector<vec2>& t) {
  /* Noise::Simplex simp(5, 0.65, 4.0/size.x, 2); */
  for (int y = 0; y < mSize.y - 1; y++) {
    vec2 s = vec2(0, y / (mSize.y - 1));
    for (int x = 0; x < mSize.x - 1; x++) {
      s.x       = x / (mSize.x - 1);
      v[y][x]   = vec3(-0.5 + s.x, 0, -0.5 + s.y);
      t[y][x]   = s;
      v[y][x].y = 0;
    }
  }
}

void GLGrid3D::generateNormals(const dVector<vec3>& v, dVector<vec3>& n) {
  dVector<vec3> normals1(mSize.y - 1, vector<vec3>(mSize.x - 1));
  dVector<vec3> normals2(mSize.y - 1, vector<vec3>(mSize.x - 1));

  for (int y = 0; y < mSize.y - 1; y++) {
    for (int x = 0; x < mSize.x - 1; x++) {
      vec3 triangleNorm0 =
        cross(v[y][x] - v[y + 1][x], v[y + 1][x] - v[y + 1][x + 1]);
      vec3 triangleNorm1 =
        cross(v[y + 1][x + 1] - v[y][x + 1], v[y][x + 1] - v[y][x]);
      normals1[y][x] = normalize(triangleNorm0);
      normals2[y][x] = normalize(triangleNorm1);
    }
  }
  for (int i = 0; i < mSize.y; i++) {
    for (int j = 0; j < mSize.x; j++) {
      vec3 finNormal = vec3(0, 0, 0);

      if (j != 0 && i != 0) {
        finNormal += normals1[i - 1][j - 1];
        finNormal += normals2[i - 1][j - 1];
      }

      if (i != 0 && j != mSize.x - 1)
        finNormal += normals1[i - 1][j];

      if (i != mSize.y - 1 && j != mSize.x - 1) {
        finNormal += normals1[i][j];
        finNormal += normals2[i][j];
      }

      if (i != mSize.y - 1 && j != 0)
        finNormal += normals2[i][j - 1];

      n[i][j] = normalize(finNormal);
    }
  }
}

void GLGrid3D::setupOpenGLArrays(const vector<Vertex>& v,
                                 const vector<int>&    i) {
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vertex) * v.size(),
               &v[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(vec3)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(vec3) + sizeof(vec2)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(int) * i.size(),
               &i[0],
               GL_STATIC_DRAW);

  glBindVertexArray(0);
}