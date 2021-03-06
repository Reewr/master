#include "Checkbox.hpp"

#include <tinyxml2.h>

#include "../GLSL/Program.hpp"
#include "../Input/Event.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Utils/Asset.hpp"
#include "Text.hpp"

Checkbox::Checkbox(const mmm::vec2& pos)
    : Logging::Log("Checkbox"), mIsTicked(false) {
  mBoundingBox = Rectangle(pos, mmm::vec2(21, 21));
  mSquare      = new GLRectangle();
  mTick        = new Text("Font::Dejavu", "", mmm::vec2(pos.x, pos.y - 3), 20);

  mTick->setColor(Text::WHITE);
  mSquare->setTexture(mAsset->rManager()->get<Texture>("Texture::Checkbox"));
  mSquare->change(mBoundingBox);
}

/**
 * @brief
 *   This loads the checkbox from XML. The syntax for the xml is:
 *
 * ```
 *   <checkbox x="50" y="20"/>
 *
 *   It can also be within menues:
 *
 *   <guimenu name="Something">
 *     <item name="SomethingElse">
 *       <checkbox x="50" y="20"/>
 *     </item>
 *   </guimenu>
 * ```
 *
 * @param element
 *
 * @return a newly allocated checkbox. Has to be deleted manually
 */
Checkbox* Checkbox::fromXML(tinyxml2::XMLElement* element) {

  mmm::vec2 position;

  if (element->QueryFloatAttribute("x", &position.x) != 0) {
    throw std::runtime_error("XMLElement has no float attribute 'x'");
  }

  if (element->QueryFloatAttribute("y", &position.y) != 0) {
    throw std::runtime_error("XMLElement has no float attribute 'y'");
  }

  return new Checkbox(position);
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
bool Checkbox::setSelected(const mmm::vec2& position) {
  bool inside = isInside(position);

  if (inside) {
    mTick->setText((mIsTicked) ? "" : "X");
    mIsTicked = !mIsTicked;
  }

  return inside;
}

/**
 * @brief
 *   Sets the offset of the checkbox and
 *   all its elements
 *
 * @param offset
 */
void Checkbox::setOffset(const mmm::vec2& offset) {
  mOffset = offset;
  mTick->setOffset(offset);
}

/**
 * @brief
 *
 *  The default handler for the Dropbox is called whenever input()
 *  is called. This can be overriden by setting a handler using
 *  setInputHandler()
 *
 *  The input handler set through this function can also
 *  call the default input handler, if the context is avaible.
 *
 * @param event
 *
 * @return
 */
void Checkbox::defaultInputHandler(const Input::Event& event) {
  if (event.buttonPressed(GLFW_MOUSE_BUTTON_LEFT))
    return;

  if (setSelected(event.position()))
    event.stopPropgation();
}

/**
 * @brief
 *   Draws the checkbox and all its elements
 */
void Checkbox::draw() {
  mGUIProgram->bind();
  mGUIProgram->setUniform("guiOffset", mOffset);
  mSquare->draw();
  mTick->draw();
}
