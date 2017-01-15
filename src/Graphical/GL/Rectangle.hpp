#pragma once

#include "../../Math/MathCD.hpp"
#include "Shape.hpp"

namespace GL {
/**
 * @brief This class serves as a simplification
 * of drawing rectangles with OpenGL
 */
class Rectangle : public Shape {
public:
  Rectangle();

  //! Creates an opengl rectangle based on the position
  //! and size information given in `rect`
  //! isCCW tells whether to utilize counter-clockwise rendering
  Rectangle(const Rect& rect, bool isCCW = true);

  //! Creates an opengl using position and size by effectively
  //! converting it to a rectangle.
  //! isCCW tells whether to utilize counter-clockwise rendering
  Rectangle(const vec2& position, const vec2& size, bool isCCW = true);
  ~Rectangle();

  //! Changes the stored rectangle to a new one. By doing some optimizations
  //! using the OpenGL buffers, this is faster than reconstructing
  //! a new one
  void change(const Rect& rect, bool isCCW = true);

  //! Changes the stored rectangle to a new one. By doing some optimizations
  //! using the OpenGL buffers, this is faster than reconstructing
  //! a new one
  void change(const vec2& position, const vec2& size, bool isCCW = true);

  //! Setup the OpenGL buffers and vertex arrays for the rectangle
  void setup();

  //! Draws the rectangle using OpenGL drawing calls
  void draw();

private:
  Rect mRect;
  bool mIsCCW;
};
}