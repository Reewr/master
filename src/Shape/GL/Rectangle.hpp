#pragma once

#include "../Rectangle.hpp"
#include "../../Resource/Texture.hpp"
#include "Shape.hpp"
#include <memory>

/**
 * @brief This class serves as a simplification
 * of drawing rectangles with OpenGL
 */
class GLRectangle : public GLShape {
public:
  GLRectangle();

  //! Creates an opengl rectangle based on the position
  //! and size information given in `rect`
  //! isCCW tells whether to utilize counter-clockwise rendering
  GLRectangle(const Rectangle& rect, bool isCCW = true);

  //! Creates an opengl using position and size by effectively
  //! converting it to a rectangle.
  //! isCCW tells whether to utilize counter-clockwise rendering
  GLRectangle(const vec2& position, const vec2& size, bool isCCW = true);
  ~GLRectangle();

  //! Changes the stored rectangle to a new one. By doing some optimizations
  //! using the OpenGL buffers, this is faster than reconstructing
  //! a new one
  void change(const Rectangle& rect, bool isCCW = true);

  //! Changes the stored rectangle to a new one. By doing some optimizations
  //! using the OpenGL buffers, this is faster than reconstructing
  //! a new one
  void change(const vec2& position, const vec2& size, bool isCCW = true);

  // Sets the texture of the rectangle, telling it to bind the texture when
  // drawing
  void setTexture(std::shared_ptr<Texture> t);

  //! Setup the OpenGL buffers and vertex arrays for the rectangle
  void setup();

  //! Draws the rectangle using OpenGL drawing calls
  void draw();

private:
  Rectangle                mRect;
  bool                     mIsCCW;
  std::shared_ptr<Texture> mTexture;
};