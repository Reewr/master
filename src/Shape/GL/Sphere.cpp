#include "Sphere.hpp"

#include <vector>

#include "../../Utils/Utils.hpp"
#include "../../GlobalLog.hpp"

using mmm::vec3;
using mmm::cos;
using mmm::sin;

int GLSphere::mSphereCounter = 0;
int GLSphere::mNumQuads = 0;
GLuint GLSphere::mIBO = 0;
GLuint GLSphere::mVBO = 0;
GLuint GLSphere::mVAO = 0;

GLSphere::GLSphere(bool outline) : mOutline(outline) {
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

GLShape::Vertex GLSphere::genVertex(float u, float v) {
  static float PI = mmm::constants<float>::pi;
  float r = sin(PI * v);
  return {
    {r * cos(2.0f * PI * u), r * sin(2.0f * PI * u), cos(PI * v)},
    {u, v},
    {0, 0, 0}
  };
}

/**
 * @brief
 *   Sets up the verticies and indicies used to draw the cube.
 *
 *   FIXME: Optimize for changes
 */
void GLSphere::setup() {

  Utils::clearGLError();

  if (mIBO == 0)
    glGenBuffers(1, &mIBO);

  Utils::assertGL();

  if (mVBO == 0)
    glGenBuffers(1, &mVBO);

  Utils::assertGL();

  if (mVAO == 0)
    glGenVertexArrays(1, &mVAO);

  Utils::assertGL();

  int rings = 10;
  int sectors = 10;
  float R  = 1.0 / float(rings - 1);
  float S  = 1.0 / float(sectors - 1);

  std::vector<GLShape::Vertex> vertices;

  vertices.reserve(rings * sectors * 6);

  for(int s = 0; s < sectors; s++) {
    for(int r = 0; r < rings; r++) {
      float u0 = r * R;
      float v0 = s * S;
      float u1 = (r + 1) * R;
      float v1 = (s + 1) * S;

      vertices.push_back(genVertex(u0, v1)); // 0
      vertices.push_back(genVertex(u1, v1)); // 1
      vertices.push_back(genVertex(u1, v0)); // 2
      vertices.push_back(genVertex(u1, v0)); // 2
      vertices.push_back(genVertex(u0, v0)); // 3
      vertices.push_back(genVertex(u0, v1)); // 0
    }
  }

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

  Utils::assertGL();

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  Utils::assertGL();

  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(mmm::vec3)));

  Utils::assertGL();

  glVertexAttribPointer(2,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(mmm::vec3) + sizeof(mmm::vec2)));

  Utils::assertGL();

  glBindVertexArray(0);

  mNumQuads = vertices.size();
}

/**
 * @brief
 *   Draws the cube using the indicies
 */
void GLSphere::draw() {
  if (mVAO == 0)
    return;

  glBindVertexArray(mVAO);

  if (mOutline)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_TRIANGLES, 0, mNumQuads);

  if (mOutline)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glBindVertexArray(0);
}
