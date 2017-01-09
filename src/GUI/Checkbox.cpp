#include "Checkbox.hpp"

#include <tinyxml2.h>

#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Utils/Asset.hpp"
#include "../GLSL/Program.hpp"

Checkbox::Checkbox(const std::string box, const vec2& pos) : mIsTicked(false) {
  mBoundingBox = Rect(pos, vec2(21, 21));
  mSquare      = new Texture(box.c_str());
  mTick        = new Text(mFont, "", vec2(pos.x, pos.y-3), 20);
  mTick->setColor(Text::WHITE);
  mSquare->recalculateGeometry(mBoundingBox);
}

/**
 * @brief
 *   This loads the checkbox from XML. The syntax for the xml is:
 *
 *   <checkbox x="50" y="20"/>
 *
 *   It can also be within menues:
 *
 *   <guimenu name="Something">
 *     <item name="SomethingElse">
 *       <checkbox x="50" y="20"/>
 *     </item>
 *   </guimenu>
 *
 * @param element
 *
 * @return a newly allocated checkbox. Has to be deleted manually
 */
Checkbox* Checkbox::fromXML(tinyxml2::XMLElement* element) {

  vec2 position;
  const char* check = element->Attribute("check");

  if (element->QueryFloatAttribute("x", &position.x) != 0) {
    throw new Error("XMLElement has no float attribute 'x'");
  }

  if (element->QueryFloatAttribute("y", &position.y) != 0) {
    throw new Error("XMLElement has no float attribute 'y'");
  }

  if (check == nullptr) {
    throw new Error("XMLElement has no 'check' attribute");
  }

  return new Checkbox(std::string(check), position);
}

Checkbox::~Checkbox() {
  delete mTick;
  delete mSquare;
}

/**
 * @brief
 *   Sets whether or not the checkbox
 *   should be selected (ticked) based
 *   on the given screen position
 *
 * @param position
 *   screen position
 */
void Checkbox::setSelected(const vec2& position) {
  if (isInside(position)) {
    mTick->setText((mIsTicked) ? "" : "X");
    mIsTicked = !mIsTicked;
  }
}

/**
 * @brief
 *   Sets the offset of the checkbox and
 *   all its elements
 *
 * @param offset
 */
void Checkbox::setOffset(const vec2& offset) {
  mOffset = offset;
  mTick->setOffset(offset);
}

/**
 * @brief
 *   Draws the checkbox and all its elements
 *
 * @param float
 */
void Checkbox::draw(float) const {
  mGUIProgram->bind();
  mGUIProgram->setUniform("guiOffset", mOffset);
  mSquare->draw();
  mTick->draw();
}