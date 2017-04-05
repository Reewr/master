#include "Cube.hpp"

#include "../../Utils/Utils.hpp"

int GLCube::mCubeCounter = 0;
GLuint GLCube::mIBO = 0;
GLuint GLCube::mVBO = 0;
GLuint GLCube::mVAO = 0;

GLCube::GLCube() {
  if (mCubeCounter == 0)
    setup();

  mCubeCounter++;
}

GLCube::~GLCube() {
  mCubeCounter--;

  if (mCubeCounter == 0) {
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
void GLCube::setup() {
  if (mIBO == 0)
    glGenBuffers(1, &mIBO);
  if (mVBO == 0)
    glGenBuffers(1, &mVBO);
  if (mVAO == 0)
    glGenVertexArrays(1, &mVAO);

  // clang-format off
  GLfloat vertices[] = {
    // front
    -0.5, -0.5,  0.5, 0.0, 1.0,
     0.5, -0.5,  0.5, 1.0, 1.0,
     0.5,  0.5,  0.5, 1.0, 0.0,
    -0.5,  0.5,  0.5, 0.0, 0.0,
    // top
    -0.5,  0.5,  0.5, 0.0, 1.0,
     0.5,  0.5,  0.5, 1.0, 1.0,
     0.5,  0.5, -0.5, 1.0, 0.0,
    -0.5,  0.5, -0.5, 0.0, 0.0,
    // back
     0.5, -0.5, -0.5, 0.0, 1.0,
    -0.5, -0.5, -0.5, 1.0, 1.0,
    -0.5,  0.5, -0.5, 1.0, 0.0,
     0.5,  0.5, -0.5, 0.0, 0.0,
    // bottom
    -0.5, -0.5, -0.5, 0.0, 1.0,
     0.5, -0.5, -0.5, 1.0, 1.0,
     0.5, -0.5,  0.5, 1.0, 0.0,
    -0.5, -0.5,  0.5, 0.0, 0.0,
    // left
    -0.5, -0.5, -0.5, 0.0, 1.0,
    -0.5, -0.5,  0.5, 1.0, 1.0,
    -0.5,  0.5,  0.5, 1.0, 0.0,
    -0.5,  0.5, -0.5, 0.0, 0.0,
    // right
     0.5, -0.5,  0.5, 0.0, 1.0,
     0.5, -0.5, -0.5, 1.0, 1.0,
     0.5,  0.5, -0.5, 1.0, 0.0,
     0.5,  0.5,  0.5, 0.0, 0.0,
  };

  GLuint elements[] = {
     // front
     0,  1,  2,
     2,  3,  0,
    // top
     4,  5,  6,
     6,  7,  4,
    // back
     8,  9, 10,
    10, 11,  8,
    // bottom
    12, 13, 14,
    14, 15, 12,
    // left
    16, 17, 18,
    18, 19, 16,
    // right
    20, 21, 22,
    22, 23, 20,
  };
  // clang-format on

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        5 * sizeof(GL_FLOAT),
                        (void*) (3 * sizeof(GL_FLOAT)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(elements),
               elements,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
}

/**
 * @brief
 *   Draws the cube using the indicies
 */
void GLCube::draw() {
  glBindVertexArray(mVAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
