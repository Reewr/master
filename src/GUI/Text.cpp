#include "Text.hpp"

#include <vector>
#include <algorithm>
#include <vector>
#include <stack>

#include "../GLSL/Program.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Resource/Font.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

using mmm::vec2;
using mmm::vec3;
using mmm::vec4;

enum ColorState {
  START,
  ESCAPED,
  FOREGROUND,
  BACKGROUND,
};

/**
 * @brief
 *   Used by parseString to find the colors in a color definition.
 *   The color definition is defined as:
 *
 *      \<R,B,G,A:R,B,G,A>TEXT\</>
 *
 *   where NUM is a number between 0 and 255 and text is any given
 *   text including another definition of colors.
 *
 *   The first four numbers is the foreground color whereas the next
 *   numbers is the background color.
 *
 *   A color definition also supports empty sections, such as
 *
 *    \<:NUM,NUM,NUM,Num>TEXT\</>
 *    \<NUM,NUM,NUM,NUM:>TEXT\</>
 *    \<:>TEXT\</>
 *
 *   The first will use the foreground color of the previous scope
 *   (default value if no previous scope has been defined). The second
 *   one will use the background of the previous scope, whereas the last
 *   one will, really, just be ignored.
 *
 *   Please don't use the last one.
 *
 *   **Note**: Remember to escape ALL the angle left facing angle brackets.
 *
 * @param parsing
 *   This is the whole string of what is being parsed so that it can check
 *   if it goes past the end.
 *
 * @param current
 *   This is an iterator to the current character in the string
 *
 * @param color
 *   This is a reference to the color, where it will use default
 *   if it cannot find any colors.
 *
 * @param sep
 *   This is the type of seperator, or, ending delimiter, such as ':' or '>'
 */
void parseColor(const std::string& parsing,
                std::string::const_iterator& current,
                vec4& color,
                char sep) {
  std::vector<std::string> colorStrs = {"", "", "", ""};
  int colorCount = 0;

  while(*current != sep) {

    if (*current == ',') {
      ++colorCount;
    } else {
      colorStrs[colorCount].push_back(*current);
    }

    if (colorCount > 3) {
      throw std::runtime_error("Too many color arguments: '" + parsing + "'");
    }

    current++;

    if (current == parsing.end()) {
      throw std::runtime_error("No ending delimiter: '" + parsing + "'");
    }
  }

  // If there are no values, assume default value
  if (colorCount == 0)
    return;

  try {
    color = {
      std::stof(colorStrs[0]) / 255.0f,
      std::stof(colorStrs[1]) / 255.0f,
      std::stof(colorStrs[2]) / 255.0f,
      std::stof(colorStrs[3]) / 255.0f
    };
  } catch (const std::invalid_argument& i) {
    throw std::runtime_error("Invalid color value: '" + parsing + "'");
  }

  if (color.x > 1 || color.y > 1 || color.z > 1 || color.w > 1) {
    throw std::runtime_error("Color value over 255: '" + parsing + "'");
  }

  if (color.x < 0 || color.y < 0 || color.z < 0 || color.y < 0) {
    throw std::runtime_error("Color value under 0: '" + parsing + "'");
  }
}

Text::Text(const std::string& font,
           const std::string& text,
           const vec2&        position,
           int                size,
           int                color,
           const vec2&        limit)
    : mNumVertices(0)
    , mCharacterSize(size)
    , mNumVerticesBackground(0)
    , mStyle(0)
    , mVBO(0)
    , mVAO(0)
    , mVBOBackground(0)
    , mVAOBackground(0)
    , mText(text)
    , mLimit(limit) {

  mTextFont    = mAsset->rManager()->get<Font>(font);
  mFontProgram = mAsset->rManager()->get<Program>("Program::Font");
  mIsLimitOn   = limit.x != 0 || limit.y != 0;
  mColor       = {vec3(), vec3(), 0, 0};

  try {
    mFormattedText = parseString(text);
    mHasBackgroundColor = false;
    for(auto f : mFormattedText)
      if (f.color.background.w != 0) {
        mHasBackgroundColor = true;
        break;
      }
  } catch (const std::runtime_error& r) {
    mFormattedText = {{{vec4(1, 1, 1, 1), vec4(0, 0, 0, 0)}, mText}};
  }

  mBoundingBox.topleft    = position;

  mFontProgram->setUniform("screenRes", mAsset->cfg()->graphics.res);

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

bool Text::hasBackgroundColor() const {
  return mHasBackgroundColor;
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
  try {
    mFormattedText = parseString(text);
    mHasBackgroundColor = false;
    for(auto f : mFormattedText)
      if (f.color.background.w != 0) {
        mHasBackgroundColor = true;
        break;
      }
  } catch (const std::runtime_error& r) {
    mFormattedText = {{{vec4(1, 1, 1, 1), vec4(0, 0, 0, 0)}, mText}};
  }
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
 *   Returns the text without any of the formatting
 *   in it. This function is not very efficient and
 *   should be used sparingly.
 *
 * @return
 */
std::string Text::getFormattedText() {
  std::string s = "";

  for (auto t : mFormattedText) {
    s += t.text;
  }

  return s;
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
 *   Used by parseString to find the colors in a color definition.
 *   The color definition is defined as:
 *
 *      \<R,B,G,A:R,B,G,A>TEXT\</>
 *
 *   where each is a number between 0 and 255 and text is any given
 *   text including another definition of colors.
 *
 *   The first four numbers is the foreground color whereas the next
 *   numbers is the background color.
 *
 *   A color definition also supports empty sections, such as
 *
 *    \<:R,B,G,A>TEXT\</>
 *    \<R,B,G,A:>TEXT\</>
 *    \<:>TEXT\</>
 *
 *   The first will use the foreground color of the previous scope
 *   (default value if no previous scope has been defined). The second
 *   one will use the background of the previous scope, whereas the last
 *   one will, really, just be ignored.
 *
 *   Please don't use the last one.
 *
 *   **Note**: Remember to escape ALL the angle left facing angle brackets.
 *
 * @param s
 *   The string to parse
 *
 * @param defaultForeground
 *   You guessed it. Default foreground color.
 *
 * @param defaultBackground
 *   Can you guess this too?
 */
std::vector<Text::TextBlock> Text::parseString(const std::string& s,
                                               vec4 defaultForeground,
                                               vec4 defaultBackround) {
  std::stack<ConsoleColor> st;
  st.push({defaultForeground, defaultBackround});

  std::vector<Text::TextBlock> colors = {};
  Text::TextBlock current = {{defaultForeground, defaultBackround}, ""};
  int state = 0;
  char lastChar;

  for (auto c = s.begin(); c != s.end(); ++c){
    switch (state) {

      // Start state represents the catch all state. Also finds
      // the starting point of color definitions
      case ColorState::START:
        if (*c == '<' && lastChar == '\\') {
          state = FOREGROUND;

          current.text.pop_back();

          colors.push_back(current);
          current = Text::TextBlock();
          current.color = st.top();
        } else {
          current.text.push_back(*c);
        }

        break;

      // Parses the foreground color of the <0,0,0:0,0,0> color, which
      // is the three first numbers.
      case ColorState::FOREGROUND:
        if (*c == '/') {
          state = START;

          if (st.size() == 1)
            throw std::runtime_error("Unbalanced color tags in '" + s + "'");
          else {
            st.pop();
            current.color = st.top();
          }

          ++c;
        } else {
          state = BACKGROUND;
          parseColor(s, c, current.color.foreground, ':');
        }

        break;

      // Parses the background color of the <0,0,0:0,0,0> color, which
      // is the three last numbers.
      case ColorState::BACKGROUND:
        state = START;
        parseColor(s, c, current.color.background, '>');
        st.push(current.color);
        break;
    }

    lastChar = *c;
  }

  if (current.text != "") {
    colors.push_back(current);
  }

  colors.erase(std::remove_if(colors.begin(), colors.end(), [](Text::TextBlock& c) {
      return c.text.size() == 0;
  }), colors.end());

  return colors;
}

/**
 * @brief
 *   Strips the color information from the string, returning
 *   a string without this information.
 *
 * @param s
 *
 * @return
 */
std::string Text::stripColorsFromStr(const std::string& s) {
  std::string text;
  try {
    auto textBlocks = Text::parseString(s);
    for (auto t : textBlocks)
      text += t.text;
  } catch (const std::runtime_error& r) {
    text = s;
  }

  return text;
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
 *   Sets the color as an RGB.
 *
 * @param color
 */
void Text::setColor(const mmm::vec3& color) {
  mColor.prevEnumColor    = mColor.currentEnumColor;
  mColor.previous         = mColor.current;
  mColor.currentEnumColor = 0;
  mColor.current          = color;
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

  std::vector<mmm::vec<8>> coordinates;
  std::vector<mmm::vec<8>> bkCords;

  float      scale   = 1;
  const vec2 texSize = mTextFont->getTexture(mCharacterSize)->getSize();
  const vec2 metrics = mTextFont->getMetrics(mCharacterSize);
  vec2       tempPos = mBoundingBox.topleft + vec2(0, mCharacterSize);

  for (auto textBlock : mFormattedText) {

    vec4& fColor = textBlock.color.foreground;
    vec4& bColor = textBlock.color.background;

    float x0;
    float xLast;

    for (unsigned int i = 0; i < textBlock.text.size(); ++i) {
      char c = textBlock.text[i];
      const Font::Glyph g = mTextFont->getGlyph(c, mCharacterSize);

      const float x2 = tempPos.x + g.bitmapLoc.x / scale;
      const float y2 = -tempPos.y + g.bitmapLoc.y / scale;

      const float w = g.bitmapSize.x / scale;
      const float h = g.bitmapSize.y / scale;

      if (i == 0)
        x0 = x2;

      xLast = x2 + w;

      tempPos.x += g.advance.x / scale;
      tempPos.y += g.advance.y / scale;

      bool isNewline    = c == '\n';
      bool isWhitespace = c == ' ';
      bool isAboveLimit =
        mIsLimitOn && tempPos.x > mBoundingBox.topleft.x + mLimit.x;

      if (isNewline || (isAboveLimit && isWhitespace)) {
        /* bkCords.push_back({ xLast, tempPos.y            , bColor }); */
        /* bkCords.push_back({ x0   , tempPos.y + metrics.y, bColor }); */
        /* bkCords.push_back({ xLast, tempPos.y + metrics.y, bColor }); */

        /* bkCords.push_back({ x0   , tempPos.y + metrics.y, bColor }); */
        /* bkCords.push_back({ xLast, tempPos.y            , bColor }); */
        /* bkCords.push_back({ x0   , tempPos.y            , bColor }); */
        tempPos.x = mBoundingBox.topleft.x;
        tempPos.y += mCharacterSize * 2;
        continue;
      } else if (isWhitespace) {
        xLast = x2 + g.advance.x;
      } else if (!w || !h) {
        continue;
      }

      const float tx = g.tc.x + g.bitmapSize.x / texSize.x;
      const float ty = g.tc.y + g.bitmapSize.y / texSize.y;

      coordinates.push_back({ x2 + w, -y2    , tx    , g.tc.y, fColor });
      coordinates.push_back({ x2    , -y2 + h, g.tc.x, ty    , fColor });
      coordinates.push_back({ x2 + w, -y2 + h, tx    , ty    , fColor });

      coordinates.push_back({ x2    , -y2 + h, g.tc.x, ty    , fColor });
      coordinates.push_back({ x2 + w, -y2    , tx    , g.tc.y, fColor });
      coordinates.push_back({ x2    , -y2    , g.tc.x, g.tc.y, fColor });

      mBoundingBox.bottomright(vec2(x2 + w, -y2 + mCharacterSize));
    }

    if (mHasBackgroundColor) {
      bkCords.push_back({ xLast, tempPos.y - metrics.y * 0.75, 0, 0, bColor });
      bkCords.push_back({ x0   , tempPos.y + metrics.y * 0.25, 0, 0, bColor });
      bkCords.push_back({ xLast, tempPos.y + metrics.y * 0.25, 0, 0, bColor });

      bkCords.push_back({ x0   , tempPos.y + metrics.y * 0.25, 0, 0,  bColor });
      bkCords.push_back({ xLast, tempPos.y - metrics.y * 0.75, 0, 0, bColor });
      bkCords.push_back({ x0   , tempPos.y - metrics.y * 0.75, 0, 0, bColor });
    }
  }

  mNumVertices = coordinates.size();

  // Create the VAO and VBO for the actual text together
  // with their colors
  if (mVAO != 0)
    glDeleteVertexArrays(1, &mVAO);
  if (mVBO != 0)
    glDeleteBuffers(1, &mVBO);

  glGenVertexArrays(1, &mVAO);
  glGenBuffers(1, &mVBO);

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(mmm::vec<8>) * coordinates.size(),
               &coordinates[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*) (2 * sizeof(GL_FLOAT)));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*) (4 * sizeof(GL_FLOAT)));
  glBindVertexArray(0);

  // No need to create lots of background color stuff if its never
  // used.
  if (!mHasBackgroundColor)
    return;

  // Create the quads for the background colors
  mNumVerticesBackground = bkCords.size();

  if (mVAOBackground != 0)
    glDeleteVertexArrays(1, &mVAOBackground);
  if (mVBOBackground != 0)
    glDeleteBuffers(1, &mVBOBackground);

  glGenVertexArrays(1, &mVAOBackground);
  glGenBuffers(1, &mVBOBackground);

  glBindVertexArray(mVAOBackground);
  glBindBuffer(GL_ARRAY_BUFFER, mVBOBackground);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(mmm::vec<8>) * bkCords.size(),
               &bkCords[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*) (2 * sizeof(GL_FLOAT)));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*) (4 * sizeof(GL_FLOAT)));
  glBindVertexArray(0);
}

/**
 * @brief
 *   Draws the text if it is visible and
 *   the font used for the text is not null
 */
void Text::draw() {
  if (!isVisible())
    return;

  mFontProgram->bind();
  mFontProgram->setUniform("guiOffset", mOffset);

  if (mHasBackgroundColor) {
    mFontProgram->setUniform("isBackground", true);

    glBindVertexArray(mVAOBackground);
    glDrawArrays(GL_TRIANGLES, 0, mNumVerticesBackground);
    glBindVertexArray(0);
  }

  mTextFont->getTexture(mCharacterSize)->bind(0);
  mFontProgram->setUniform("isBackground", false);

  glBindVertexArray(mVAO);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
  glBindVertexArray(0);
}