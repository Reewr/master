#include "Tooltip.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

Tooltip::Tooltip() {
  mCFG = mAsset->cfg();
  mBoundingBox = Rect(0, 0, 100, 60);
  mOffset      = vec2(0, 0);
  mBackground  = new GL::Rectangle(mBoundingBox);
  mActiveText  = new Text("Font::Dejavu",
                         "",
                         mBoundingBox.topleft,
                         10,
                         Text::WHITE,
                         mBoundingBox.size);
  mBackground->setTexture(
    mAsset->rManager()->get<Texture>("Texture::Background"));
}

/**
 * @brief
 *   Shows a tooltip at a specific position with the text
 *   given. This will make sure that the tooltip
 *   is not displayed outside the screen size.
 *
 * @param text
 * @param pos
 */
void Tooltip::show(std::string text, const vec2& pos) {
  if (text == "")
    return;

  vec2 newPos = pos;

  int resX      = mCFG->graphics.res.x;
  int resY      = mCFG->graphics.res.y;
  int halfSizeX = mBoundingBox.size.x * 0.5;
  int halfSizeY = mBoundingBox.size.y * 0.5;

  if (newPos.x > resX - halfSizeX)
    newPos.x = resX - mBoundingBox.size.x;

  if (newPos.x < halfSizeX)
    newPos.x = mBoundingBox.size.x;

  if (newPos.y < resY - halfSizeY)
    newPos.y = resY - mBoundingBox.size.y;

  if (newPos.y < halfSizeY)
    newPos.y = mBoundingBox.size.y;

  mActiveText->setText(text);
  setOffset(newPos);
}

/**
 * @brief
 *   Sets the offset of itself and its elements
 *
 * @param offset
 */
void Tooltip::setOffset(const vec2& offset) {
  mOffset = offset;
  mActiveText->setOffset(offset);
}

/**
 * @brief
 *   Draws the elements
 *
 * @param float
 */
void Tooltip::draw() {
  mGUIProgram->bind();
  mGUIProgram->setUniform("guiOffset", mOffset);
  mBackground->draw();
  mActiveText->draw();
}
