#include "Text.hpp"

#include <vector>

#include "../GLSL/Program.hpp"
#include "../Graphical/Font.hpp"
#include "../Utils/CFG.hpp"

Text::Text(Font*              font,
           const std::string& text,
           const vec2&        position,
           int                size,
           int                color,
           const vec2&        limit) {
  mTextFont  = font;
  mLimit     = limit;
  mIsLimitOn = limit.x != 0 || limit.y != 0;
  mText      = text;
  mVBO       = 0;
  mVAO       = 0;

  mBoundingBox.topleft    = position;
  mCharacterSize          = size;
  mColor.current          = vec3();
  mColor.previous         = vec3();
  mColor.currentEnumColor = 0;
  mColor.prevEnumColor    = 0;

  isVisible(true);
  setColor(color);
  recalculateGeometry();
}

Text::~Text() {
  if (mVBO != 0)
    glDeleteBuffers(1, &mVBO);

  if (mVAO != 0)
    glDeleteVertexArrays(1, &mVAO);
}

/**
 * @brief
 *   Sets the style of the text to either
 *   bold, italic, normal using Text::Style
 *
 * @param style
 */
void Text::setStyle(int style) {
  mStyle = style;
  recalculateGeometry();
}

/**
 * @brief
 *   Sets the limit of the text so that it
 *   tries its hardest to not go above the given
 *   length / height. However, it will not break
 *   words.
 *
 * @param limit
 */
void Text::setLimit(const vec2& limit) {
  mLimit     = limit;
  mIsLimitOn = limit.x != 0 && limit.y != 0;
  recalculateGeometry();
}

/**
 * @brief
 *   Sets the text.
 *
 * @param text
 */
void Text::setText(const std::string& text) {
  if (mText == text)
    return;

  mText = text;
  recalculateGeometry();
}

/**
 * @brief
 *   Sets the size of the text used. has to
 *   be above 1.
 *
 * @param charSize
 */
void Text::setTextSize(int charSize) {
  if (charSize == mCharacterSize || charSize < 1)
    return;

  mCharacterSize = charSize;
  recalculateGeometry();
}

/**
 * @brief
 *   Sets the position of the text
 *
 * @param position
 */
void Text::setPosition(const vec2& position) {
  mBoundingBox.topleft = position;
  recalculateGeometry();
}

/**
 * @brief
 *   Returns the text as a string
 *
 * @return text
 */
std::string Text::getText() {
  return mText;
}

/**
 * @brief
 *   Retrieves the character size
 *
 * @return size
 */
int Text::getCharSize() {
  return mCharacterSize;
}

/**
 * @brief
 *   Sets the color of the text where color is
 *   a Text::Color enum. If the color does not
 *   exist, it sets the color to white
 *
 * @param color
 */
void Text::setColor(int color) {
  if (color == mColor.currentEnumColor)
    return;

  vec3 newColor;

  switch (color) {
    case BLACK:
      newColor = vec3(0.0, 0.0, 0.0);
      break;
    case WHITE:
      newColor = vec3(1.0, 1.0, 1.0);
      break;
    case RED:
      newColor = vec3(1.0, 0.0, 0.0);
      break;
    case GREEN:
      newColor = vec3(0.0, 1.0, 0.0);
      break;
    case BLUE:
      newColor = vec3(0.0, 0.0, 1.0);
      break;
    case YELLOW:
      newColor = vec3(1.0, 1.0, 0.0);
      break;
    default:
      newColor = vec3(1.0, 1.0, 1.0);
      break;
  }

  mColor.prevEnumColor    = mColor.currentEnumColor;
  mColor.previous         = mColor.current;
  mColor.currentEnumColor = color;
  mColor.current          = newColor;
}

/**
 * @brief
 *   Whenever a color is changed, the previous color is
 *   stored. This sets the color back to the previous one,
 *   this can be useful when setting the color to something
 *   distracting such as red and then back to the normal color.
 *
 *   Or when the text is being highlighted due to mouse movements.
 */
void Text::setPrevColor() {
  int  icur               = mColor.currentEnumColor;
  vec3 vcur               = mColor.current;
  mColor.current          = mColor.previous;
  mColor.currentEnumColor = mColor.prevEnumColor;
  mColor.prevEnumColor    = icur;
  mColor.previous         = vcur;
}

/**
 * @brief
 *   This function is called whenever something about
 *   the text changes. It readjusts the positions
 *   that are used to draw the text
 */
void Text::recalculateGeometry() {
  if (mTextFont == nullptr)
    return;

  std::vector<vec4> coordinates;

  float scale = 1;
  const vec2 texSize = mTextFont->getTextureSize(mCharacterSize);
  vec2       tempPos = mBoundingBox.topleft + vec2(0, mCharacterSize);

  for (char c : mText) {
    const Font::Glyph g = mTextFont->getGlyph(c, mCharacterSize);

    const float x2 = tempPos.x + g.bitmapLoc.x / scale;
    const float y2 = -tempPos.y + g.bitmapLoc.y / scale;

    const float w = g.bitmapSize.x / scale;
    const float h = g.bitmapSize.y / scale;

    tempPos.x += g.advance.x / scale;
    tempPos.y += g.advance.y / scale;

    bool isNewline    = c == '\n';
    bool isWhitespace = c == ' ';
    bool isAboveLimit =
      mIsLimitOn && tempPos.x > mBoundingBox.topleft.x + mLimit.x;

    if (isNewline || (isAboveLimit && isWhitespace)) {
      tempPos.x = mBoundingBox.topleft.x;
      tempPos.y += mCharacterSize * 2;
      continue;
    } else if (!w || !h) {
      continue;
    }

    const float tx = g.tc.x + g.bitmapSize.x / texSize.x;
    const float ty = g.tc.y + g.bitmapSize.y / texSize.y;

    coordinates.push_back({ x2 + w, -y2    , tx    , g.tc.y });
    coordinates.push_back({ x2    , -y2 + h, g.tc.x, ty });
    coordinates.push_back({ x2 + w, -y2 + h, tx    , ty });

    coordinates.push_back({ x2    , -y2 + h, g.tc.x, ty });
    coordinates.push_back({ x2 + w, -y2    , tx    , g.tc.y });
    coordinates.push_back({ x2    , -y2    , g.tc.x, g.tc.y });

    mBoundingBox.bottomright(vec2(x2 + w, -y2 + mCharacterSize));
  }

  mNumVertices = coordinates.size();

  if (mVAO != 0)
    glDeleteVertexArrays(1, &mVAO);
  if (mVBO != 0)
    glDeleteBuffers(1, &mVBO);

  glGenVertexArrays(1, &mVAO);
  glGenBuffers(1, &mVBO);

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(vec4) * coordinates.size(),
               &coordinates[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        4 * sizeof(GL_FLOAT),
                        (void*) (2 * sizeof(GL_FLOAT)));
  glBindVertexArray(0);
}

/**
 * @brief
 *   Draws the text if it is visible and
 *   the font used for the text is not null
 */
void Text::draw() {
  if (!isVisible() || mTextFont == nullptr)
    return;

  mGUIProgram->bind();
  mGUIProgram->setUniform("isText", true);
  mGUIProgram->setUniform("guiColor", mColor.current);
  mGUIProgram->setUniform("guiOffset", mOffset);

  mTextFont->getTexture(mCharacterSize).bind(0);

  glBindVertexArray(mVAO);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
  glBindVertexArray(0);
  mGUIProgram->setUniform("isText", false);
}