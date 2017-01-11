#include "Rectangle.hpp"

#include <vector>

GL::Rectangle::Rectangle() {}

GL::Rectangle::Rectangle(const Rect& r, bool isCCW) {
  change(r, isCCW);
}

GL::Rectangle::Rectangle(const vec2& p, const vec2& s, bool isCCW) {
  change(Rect(p, s), isCCW);
}

GL::Rectangle::~Rectangle() {
  glDeleteBuffers(1, &IBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}
/**
 * @brief
 *   Changes the rectangle, effectively remaking it. However, due to
 *   optimizations done in setup(), this is better than recreating
 *   the rectangle from scratch.
 *
 * @param rect
 *   The rectangle to use instead of the stored one
 *
 * @param isCCW
 *   Whether or not this should utilize counter-clockwise rendering
 */
void GL::Rectangle::change(const Rect& rect, bool isCCW) {
  mRect  = rect;
  mIsCCW = isCCW;
  setup();
}

/**
 * @brief
 *   Changes the rectangle, effectively remaking it. However, due to
 *   optimizations done in setup(), this is better than recreating
 *   the rectangle from scratch.
 *
 * @param position
 *   The start position of the rectangle, think of it as the lower
 *   corners of the rectangle.
 *
 * @param size
 *   The size of the rectangle, meaning position.x + size.x is
 *   the opposite corner of position
 *
 * @param isCCW
 *   Whether or not this should utilize counter-clockwise rendering
 */
void GL::Rectangle::change(const vec2& p, const vec2& s, bool isCCW) {
  change(Rect(p, s), isCCW);
}

/**
 * @brief
 *   Sets up the verticies and indices used to draw the rectangle.
 *   Used when the Rectangle is either created or updated with
 *   new information
 *
 * FIXME: Optimize
 */
void GL::Rectangle::setup() {
  if (IBO != 0)
    glDeleteBuffers(1, &IBO);
  if (VBO != 0)
    glDeleteBuffers(1, &VBO);
  if (VAO != 0)
    glDeleteVertexArrays(1, &VAO);

  std::vector<vec4> coords;

  if (mIsCCW) {
    coords.push_back(vec4(0, 0, 1, 0));
    coords.push_back(vec4(1, 1, 0, 1));
  } else {
    coords.push_back(vec4(0, 1, 1, 1));
    coords.push_back(vec4(1, 0, 0, 0));
  }

  GLfloat vertices[] = { mRect.topleft.x,       mRect.bottomright().y,
                         coords[0].x,           coords[0].y,
                         mRect.bottomright().x, mRect.bottomright().y,
                         coords[0].z,           coords[0].w,
                         mRect.bottomright().x, mRect.topleft.y,
                         coords[1].x,           coords[1].y,
                         mRect.topleft.x,       mRect.topleft.y,
                         coords[1].z,           coords[1].w };

  GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

  glGenBuffers(1, &IBO);
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        4 * sizeof(GL_FLOAT),
                        (void*) (2 * sizeof(GL_FLOAT)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(elements),
               elements,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
}

/**
 * @brief
 *   Performs the OpenGL draw call for the rectangle by
 *   binding the vertex array and drawing the elements
 */
void GL::Rectangle::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}