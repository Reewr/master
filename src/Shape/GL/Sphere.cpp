#include "Sphere.hpp"

#include <vector>

#include "../../Utils/Utils.hpp"

int GLSphere::mSphereCounter = 0;
int GLSphere::mNumQuads = 0;
GLuint GLSphere::mIBO = 0;
GLuint GLSphere::mVBO = 0;
GLuint GLSphere::mVAO = 0;

GLSphere::GLSphere() {
  if (mSphereCounter == 0)
    setup();

  mSphereCounter++;
}

GLSphere::~GLSphere() {
  mSphereCounter--;

  if (mSphereCounter == 0) {
    glDeleteBuffers(1, &mIBO);
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
    mNumQuads = 0;
    mIBO = 0;
    mVBO = 0;
    mVAO = 0;
  }
}

/**
 * @brief
 *   Sets up the verticies and indicies used to draw the cube.
 *
 *   FIXME: Optimize for changes
 */
void GLSphere::setup() {
  if (mIBO == 0)
    glGenBuffers(1, &mIBO);
  if (mVBO == 0)
    glGenBuffers(1, &mVBO);
  if (mVAO == 0)
    glGenVertexArrays(1, &mVAO);

  float radius = 1;
  int rings = 10;
  int sectors = 10;
  float R  = 1.0 / float(rings - 1);
  float S  = 1.0 / float(sectors - 1);
  float PI  = mmm::constants<float>::pi;
  float PI2 = PI / 2.0;

  std::vector<GLShape::Vertex> vertices;
  std::vector<GLushort> indices;

  vertices.reserve(rings * sectors);
  indices.reserve(rings * sectors * 4);

  for(int r = 0; r < rings; r++) {
    for(int s = 0; s < sectors; s++) {

      float sPi = 2*PI * s * S;
      float rPi = PI * r * S;

      float x = cos(sPi) * sin(rPi);
      float y = sin(-PI2 + PI * r * R);
      float z = sin(sPi) * sin(rPi);

      vertices.push_back({
        {x * radius, y * radius, z * radius}, // Position
        {s * S, r * R}, // Texcoords
        {x, y, z} // normals
      });

      indices.push_back(r * sectors + s);
      indices.push_back(r * sectors + s + 1);
      indices.push_back((r + 1) * sectors + s + 1);
      indices.push_back((r + 1) * sectors + s);
    }
  }

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(mmm::vec3)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(mmm::vec3) + sizeof(mmm::vec2)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLushort) * indices.size(),
               &indices[0],
               GL_STATIC_DRAW);

  glBindVertexArray(0);

  mNumQuads = indices.size() / 4;
}

/**
 * @brief
 *   Draws the cube using the indicies
 */
void GLSphere::draw() {
  if (mVAO == 0)
    return;

  glBindVertexArray(mVAO);
  glDrawElements(GL_QUADS, mNumQuads, GL_UNSIGNED_SHORT, 0);
  glBindVertexArray(0);
}
