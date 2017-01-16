#pragma once

#include <string>

#include "../OpenGLHeaders.hpp"

#include "../GUI/GUI.hpp"
#include "../Math/MathCD.hpp"

class Font;
class CFG;
class Program;

//! Text class needs a pointer to a Font object to work.
class Text : public GUI {
public:
  //! Enums for Text Color - use these if you want to change color of the text
  enum { BLACK, WHITE, RED, GREEN, BLUE, YELLOW };

  //! Enums for styles. Currently none are implemented.
  enum { BOLD, UNDERLINE, ITALIC };

  //! Only takes a pointer to a Font. Do NOT destroy the font object.
  Text(Font*              font,
       const std::string& text,
       const vec2&        position,
       int                size  = 30,
       int                color = BLACK,
       const vec2&        limit = vec2());

  //! Deconstructor
  ~Text();

  //! Draws everything using the font.
  void draw();

  //! Does nothing for the moment
  void setStyle(int s);

  //! Sets the size limit for text (Will add newlines at whitespace)
  void setLimit(const vec2& li);

  //! Changes the text. Has to recalculate vertices.
  void setText(const std::string& s);

  //! Changes size. Note: If font does not have the size, creates it. Taxing
  void setTextSize(int charSize);

  //! Changes the color on next rendering (and from there on.)
  void setColor(int c);

  //! Changes the color back to what it was previously or black.
  void setPrevColor();

  //! Changes the font. Will need to recalculate everything
  void setFont(Font* font);

  //! Changes the position and recalculates Geometry
  void setPosition(const vec2& pos);

  //! :)
  std::string getText();

  //! Returns char size
  int getCharSize();

private:
  void recalculateGeometry();

  struct {
    vec3 current;
    vec3 previous;
    int  currentEnumColor;
    int  prevEnumColor;
  } mColor;

  int mNumVertices;
  int mCharacterSize;
  int mStyle;

  GLuint mVBO;
  GLuint mVAO;

  Font* mTextFont;

  std::string mText;

  vec2 mLimit;
  bool mIsLimitOn;
};
