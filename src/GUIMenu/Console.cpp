#include "Console.hpp"

#include "../OpenGLHeaders.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input.hpp"
#include "../Math/Math.hpp"
#include "../State/State.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

GConsole::GConsole(Input* input) {
  mCurrentText = "";
  mInput       = input;
  mBoundingBox = Rect(25, 25, 768, 400);
  mRect        = new GL::Rectangle(mBoundingBox);
  mText        = new Text(mFont, "> _", vec2(30, 395), 20, Text::WHITE);

  // Specific program for the console since the console
  // is just drawn in black with alpha
  mProgram = new Program("shaders/GUI/ColorRect.vsfs", 0);
  mProgram->bindAttribs({ "position", "texcoord" }, { 0, 1 });
  mProgram->link();
  mProgram->setUniform("screenRes", mCFG->graphics.res, "guiOffset", vec2());
  mProgram->setUniform("guiColor", vec4(0, 0, 0, 0.7));
}

GConsole::~GConsole() {
  delete mProgram;
  delete mRect;
}

/**
 * @brief
 *   Handles the key input for a console
 *
 * @param key
 *   The glfw key code of the key that was pressed
 *
 * @param action
 *   The action taken
 *
 * @param mods
 *   A bit flagged int with the mods that are clicked
 *
 * @return
 */
int GConsole::handleKeyInput(const int key, const int, const int mods) {
  if (!isVisible() && Input::keyStrings[key] == "|") {
    isVisible(true);
    return State::NOCHANGE;
  } else if (!isVisible()) {
    return State::NOCHANGE;
  }


  if (key == GLFW_KEY_ESCAPE) {
    isVisible(false);
    return State::NOCHANGE;
  }


  bool isBackspace = key == Input::keyMap["backspace"];

  if (!isBackspace && (key < 46 || key > 90))
    return State::NOCHANGE;

  bool        hasShift  = mods & GLFW_MOD_SHIFT;
  std::string character = Input::keyStrings[key];

  switch (key) {
    case GLFW_KEY_BACKSPACE:
      if (mCurrentText.length() > 0)
        mCurrentText.pop_back();
    case GLFW_KEY_ENTER:
    // performCommand();
    default:
      mCurrentText.append(hasShift ? Utils::toUpper(character) : character);
  }

  mText->setText("> " + mCurrentText + "_");
  return State::NOCHANGE;
}

void GConsole::draw(float) {
  if (!isVisible())
    return;

  mProgram->bind();
  mRect->draw();
  mText->draw();
}