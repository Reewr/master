#pragma once

#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include "GUI.hpp"

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
  Text(const std::string& font,
       const std::string& text,
       const mmm::vec2&   position,
       int                size  = 30,
       int                color = BLACK,
       const mmm::vec2&   limit = mmm::vec2());

  //! Deconstructor
  ~Text();

  //! Draws everything using the font.
  void draw();

  //! Does nothing for the moment
  void setStyle(int s);

  //! Sets the size limit for text (Will add newlines at whitespace)
  void setLimit(const mmm::vec2& li);

  //! Changes the text. Has to recalculate vertices.
  void setText(const std::string& s);

  //! Changes size. Note: If font does not have the size, creates it. Taxing
  void setTextSize(int charSize);

  //! Changes the color on next rendering (and from there on.)
  void setColor(int c);

  //! Set an RGB color
  void setColor(const mmm::vec3& color);

  //! Changes the color back to what it was previously or black.
  void setPrevColor();

  //! Changes the font. Will need to recalculate everything
  void setFont(Font* font);

  //! Changes the position and recalculates Geometry
  void setPosition(const mmm::vec2& pos);

  // Checks if the text has background colors defined
  bool hasBackgroundColor() const;

  //! :)
  std::string getText();

  // Returns the text without all the color bullshit
  std::string getFormattedText();

  //! Returns char size
  int getCharSize();

private:
  struct ConsoleColor {
    mmm::vec4 foreground;
    mmm::vec4 background;
  };

  struct TextBlock {
    ConsoleColor color;
    std::string text = "";
  };

  void recalculateGeometry();

  std::vector<TextBlock>
  parseString(const std::string& s,
              mmm::vec4          defaultForeground = { 1, 1, 1, 1 },
              mmm::vec4          defaultBackround  = { 0, 0, 0, 0 });

  struct {
    mmm::vec3 current;
    mmm::vec3 previous;
    int       currentEnumColor;
    int       prevEnumColor;
  } mColor;

  int mNumVertices;
  int mCharacterSize;
  int mNumVerticesBackground;
  int mStyle;

  GLuint mVBO;
  GLuint mVAO;

  GLuint mVBOBackground;
  GLuint mVAOBackground;

  std::shared_ptr<Font> mTextFont;
  std::shared_ptr<Program> mFontProgram;

  std::vector<TextBlock> mFormattedText;
  std::string mText;

  mmm::vec2 mLimit;
  bool      mIsLimitOn;
  bool      mHasBackgroundColor;
};
