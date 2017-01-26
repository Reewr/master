#include "Slider.hpp"

#include <tinyxml2.h>

#include "Text.hpp"

#include "../GLSL/Program.hpp"
#include "../Input/Event.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Utils/Asset.hpp"

using mmm::vec2;

Slider::Slider(const vec2& pos, const float scale, const std::string& valSign) {
  mBackground = new GLRectangle();
  mButton     = new GLRectangle();
  mInfo       = new Text("Font::Dejavu", "", pos, 15, Text::WHITE);
  mScale      = scale;
  mValSign    = valSign;

  auto texBackground = mAsset->rManager()->get<Texture>("Texture::Slider");
  auto texButton = mAsset->rManager()->get<Texture>("Texture::SliderButton");

  mBoundingBox = Rectangle(pos, texBackground->getSize() * scale);
  mButtonRect  = Rectangle(pos, texButton->getSize() * scale);
  mButtonRect.topleft.y -= mButtonRect.size.y / 2;

  mBackground->change(mBoundingBox);
  mButton->change(mButtonRect);

  mBackground->setTexture(texBackground);
  mButton->setTexture(texButton);

  mButtonOffset = vec2(0, 0);
  isVisible(true);
}

Slider* Slider::fromXML(tinyxml2::XMLElement* element) {
  if (element == nullptr) {
    throw std::runtime_error("XMLElement is null");
  }

  vec2        position;
  float       scale;
  const char* valueSign = element->Attribute("scale");

  if (element->QueryFloatAttribute("x", &position.x) != 0) {
    throw std::runtime_error("XMLElement has no float attribute 'x'");
  }

  if (element->QueryFloatAttribute("y", &position.y) != 0) {
    throw std::runtime_error("XMLElement has no float attribute 'x'");
  }

  if (element->QueryFloatAttribute("x", &scale) != 0) {
    scale = 0;
  }

  if (valueSign == nullptr) {
    return new Slider(position, scale);
  }

  return new Slider(position, scale, std::string(valueSign));
}

Slider::~Slider() {
  delete mBackground;
  delete mButton;
  delete mInfo;
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
void Slider::defaultInputHandler(const Input::Event& e) {
  if (!isVisible())
    return;

  if (e.buttonPressed(GLFW_MOUSE_BUTTON_LEFT) && moveSlider(e.position())) {
    e.stopPropgation();
  }
}

/**
 * @brief
 *   Sets the offset of the slider and all
 *   its sub elements
 *
 * @param offset
 */
void Slider::setOffset(const vec2& offset) {
  mOffset = offset;
  mInfo->setOffset(offset);
}

/**
 * @brief
 *   Move the slider based on the position, restricting
 *   it to be within the limits of the slider itself.
 *
 *   Also sets the new value that is gained by moving
 *   the slider
 *
 * @param position
 *   screen positions
 *
 * @return the new value
 */
bool Slider::moveSlider(const vec2& position) {
  if (!isInside(position)) {
    return false;
  }

  int x = position.x;

  // limit x to the size of the slider
  x = mmm::max(mmm::min(x, mBoundingBox.bottomright().x), mBoundingBox.topleft.x);

  // Move the slider
  mButtonOffset.x = x - mBoundingBox.topleft.x - mButtonRect.size.x / 2;

  // Find out what value we should display
  float value =
    mButtonOffset.x / (mBoundingBox.size.x - mButtonRect.size.x / 2);
  setSlider(mmm::min(mmm::max(value, 0), 1));
  hasChanged(true);

  return true;
}

/**
 * @brief
 *   Sets the slider to a specific value between 0 and 1.
 *   Think of it as percentage where 1 is all the way right
 *   and 0 all the way left.
 *
 * @param value
 */
void Slider::setSlider(float value) {
  if (value < 0 || value > 1)
    return;

  float pos     = value * (mBoundingBox.size.x - mButtonRect.size.x / 2);
  mButtonOffset = vec2(pos, mButtonOffset.y);
  mInfo->setPosition(vec2(mBoundingBox.bottomright().x + 10,
                          mBoundingBox.topleft.y - mBoundingBox.size.y / 2));
  mInfo->setText(std::to_string(static_cast<int>(value * 100)) + mValSign);
  mValue = value;
}

/**
 * @brief
 *   Retrieves the stored value for the slider
 *
 * @return
 */
float Slider::value() const {
  return mValue;
}

/**
 * @brief
 *   Draws the elements
 *
 * @param float
 */
void Slider::draw() {
  if (!mIsVisible)
    return;

  mGUIProgram->bind();
  mGUIProgram->setUniform("guiOffset", mOffset);
  mBackground->draw();
  mGUIProgram->setUniform("guiOffset", mOffset + mButtonOffset);
  mButton->draw();
  mInfo->draw();
}
