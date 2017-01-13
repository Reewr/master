#include "Inputbox.hpp"

#include <tinyxml2.h>

#include "../GLSL/Program.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

Inputbox::Inputbox(const Rect& r, const std::string text) {
  mTextBox  = new Texture(TEMP::getPath(TEMP::OPTSMENU));
  mInputBox = new Texture(TEMP::getPath(TEMP::SLIDER));

  mInputBoxRect = Rect(vec2(mCFG->graphics.res.x * 0.50 - 200,
                            mCFG->graphics.res.y * 0.50 - 25),
                       vec2(400, 50));
  mBoundingBox = r;

  mText = new Text(mFont, text, { 0, 0 }, 15);
  mText->setColor(Text::WHITE);

  vec2 textPos(mBoundingBox.middle().x - mText->box().middle().x,
               mBoundingBox.middle().y - mText->box().middle().y);

  mText->setPosition(textPos);

  mInputBoxText = new Text(mFont, "Please input keybinding", vec2(0, 0), 15);
  mInputBoxText->setColor(Text::WHITE);
  textPos =
    vec2(mInputBoxRect.topleft.x + 200 - mInputBoxText->box().middle().x,
         mInputBoxRect.topleft.y);

  mInputBoxText->setPosition(textPos);
  mTextBox->recalculateGeometry(mBoundingBox);
  mInputBox->recalculateGeometry(mInputBoxRect);
  mInputIsVisible = false;
  hasChanged(false);
}

/**
 * @brief
 *   Creates an input box from xml
 *
 *   Syntax:
 *
 *   <inputbox text="Some text" x1="50" y1="50" x2="100" y2="100"/>
 *
 * @param element
 *
 * @return
 */
Inputbox* Inputbox::fromXML(tinyxml2::XMLElement* element) {
  if (element == nullptr) {
    throw Error("XMLElement is null");
  }

  Rect        rect;
  const char* text = element->Attribute("text");

  if (text == nullptr) {
    throw Error("XMLElement has no attribute 'text'");
  }

  if (element->QueryFloatAttribute("x1", &rect.topleft.x) != 0) {
    throw Error("XMLElement has no float attribute 'x1'");
  }

  if (element->QueryFloatAttribute("y1", &rect.topleft.y) != 0) {
    throw Error("XMLElement has no float attribute 'y1'");
  }

  if (element->QueryFloatAttribute("x2", &rect.size.x) != 0) {
    throw Error("XMLElement has no float attribute 'x2'");
  }

  if (element->QueryFloatAttribute("y2", &rect.size.x) != 0) {
    throw Error("XMLElement has no float attribute 'y2'");
  }

  return new Inputbox(rect, std::string(text));
}

Inputbox::~Inputbox() {
  delete mTextBox;
  delete mInputBox;
  delete mText;
  delete mInputBoxText;
}

/**
 * @brief
 *   Changes the text to given text. Does not change the input
 *   if the input is not visible and force change is false.
 *
 * @param text
 * @param forceChange
 *
 * @return true if the text was changed
 */
bool Inputbox::changeText(const std::string text, const bool forceChange) {
  if (!mInputIsVisible && !forceChange)
    return false;

  if (mText->getText() != text)
    hasChanged(true);

  mText->setPosition(vec2(0, 0));
  mText->setText(text);
  mText->setPosition(
    vec2(mBoundingBox.middle().x - mText->box().bottomright().x / 2,
         mBoundingBox.middle().y - mText->box().bottomright().y / 2));

  mInputIsVisible = false;
  return true;
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
void Inputbox::defaultInputHandler(const Input::Event& event) {
  if (event.buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {

    if (showInputbox(event.position()))
      event.stopPropgation();

    return;
  }

  if (event == Input::Event::Type::KeyPress && mInputIsVisible) {
    if (changeText(Input::glfwKeyToString(event.key())))
      event.stopPropgation();
    return;
  }

  return;
}

/**
 * @brief
 *   Returns the text stored in the input
 *
 * @return
 */
std::string Inputbox::text() const {
  return mText->getText();
}

/**
 * @brief
 *   If the position is inside the the input, it
 *   shows input
 *
 * @param position
 */
bool Inputbox::showInputbox(const vec2& position) {
  mInputIsVisible = isInside(position);

  return mInputIsVisible;
}

/**
 * @brief
 *   Sets the offset of the inputbox and its sub
 *   elements
 *
 * @param offset
 */
void Inputbox::setOffset(const vec2& offset) {
  mOffset = offset;
  mText->setOffset(offset);
  mInputBoxText->setOffset(offset);
}

/**
 * @brief
 *   Draws the input box and the other sub elements
 *
 * @param float
 */
void Inputbox::draw(float) {
  mGUIProgram->bind();
  mGUIProgram->setUniform("guiOffset", mOffset);

  mTextBox->draw();
  mText->draw();

  if (mInputIsVisible) {
    mGUIProgram->bind();
    mGUIProgram->setUniform("guiOffset", mOffset);
    mInputBox->draw();
    mInputBoxText->draw();
  }
}
