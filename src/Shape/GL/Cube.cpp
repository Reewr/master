#include "Cube.hpp"

#include "../../Utils/Utils.hpp"

GLCube::GLCube(const mmm::vec2& size) : mSize(size) {
  setup();
}

GLCube::~GLCube() {
  glDeleteBuffers(1, &IBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

/**
 * @brief
 *   Sets up the verticies and indicies used to draw the cube.
 *
 *   FIXME: Optimize for changes
 */
void GLCube::setup() {
  if (IBO == 0)
    glGenBuffers(1, &IBO);
  if (VBO == 0)
    glGenBuffers(1, &VBO);
  if (VAO == 0)
    glGenVertexArrays(1, &VAO);

  // clang-format off
  GLfloat vertices[] = { // front
                         -1.0, -1.0, 1.0,
                          1.0, -1.0, 1.0,
                          1.0,  1.0, 1.0,
                         -1.0,  1.0, 1.0,

                         // back
                         -1.0, -1.0, -1.0,
                          1.0, -1.0, -1.0,
                          1.0,  1.0, -1.0,
                         -1.0,  1.0, -1.0 };

  GLuint elements[] = {
    // front
    0, 1, 2,
    2, 3, 0,

    // top
    1, 5, 6,
    6, 2, 1,

    // back
    7, 6, 5,
    5, 4, 7,

    // bottom
    4, 0, 3,
    3, 7, 4,

    // left
    4, 5, 1,
    1, 0, 4,

    // right
    3, 2, 6,
    6, 7, 3
  };
  // clang-format on

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
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
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
