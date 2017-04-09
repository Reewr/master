#include "Line.hpp"

#include "../../Utils/Utils.hpp"
#include <vector>

using mmm::vec3;
using mmm::cos;
using mmm::sin;

int    GLLine::mLineCounter = 0;
GLuint GLLine::mIBO         = 0;
GLuint GLLine::mVBO         = 0;
GLuint GLLine::mVAO         = 0;

GLLine::GLLine() {
  if (mLineCounter == 0)
    setup();

  mLineCounter++;
}

GLLine::~GLLine() {
  mLineCounter--;

  if (mLineCounter == 0) {
    glDeleteBuffers(1, &mIBO);
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
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
void GLLine::setup() {

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

  std::vector<Vertex>
    vertices{ { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
              { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } } };

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vertex) * vertices.size(),
               &vertices[0],
               GL_STATIC_DRAW);

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

  /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO); */
  /* glBufferData(GL_ELEMENT_ARRAY_BUFFER, */
  /*              sizeof(GLushort) * indices.size(), */
  /*              &indices[0], */
  /*              GL_STATIC_DRAW); */

  Utils::assertGL();

  glBindVertexArray(0);
}

/**
 * @brief
 *   Draws the cube using the indicies
 */
void GLLine::draw() {
  if (mVAO == 0)
    return;

  glBindVertexArray(mVAO);

  Utils::assertGL();
  glDrawArrays(GL_LINES, 0, 2);
  Utils::assertGL();

  glBindVertexArray(0);
}
