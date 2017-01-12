#include "Console.hpp"

#include "../OpenGLHeaders.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input/Input.hpp"
#include "../Math/Math.hpp"
#include "../State/State.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

Console::Console(Input::Input* input) {
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

Console::~Console() {
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
int Console::handleKeyInput(const int key, const int, const int mods) {
  if (!isVisible() && mInput->checkKey(Input::Action::Console, key)) {
    isVisible(true);
    return State::HANDLED_INPUT;
  } else if (!isVisible()) {
    return State::NOCHANGE;
  }

  if (key == GLFW_KEY_ESCAPE) {
    isVisible(false);
    return State::HANDLED_INPUT;
  }

  bool isBackspace = key == GLFW_KEY_BACKSPACE;

  if (!isBackspace && (key < 46 || key > 90))
    return State::NOCHANGE;

  bool hasShift = mods & GLFW_MOD_SHIFT;

  switch (key) {
    case GLFW_KEY_BACKSPACE:
      if (mCurrentText.length() > 0)
        mCurrentText.pop_back();
      break;
    case GLFW_KEY_ENTER:
      log("Info should perform action with '" + mCurrentText + "'");
      break;
    // performCommand();
    default:
      mCurrentText.append(hasShift ? Utils::toUpper("") : "");
  }

  mText->setText("> " + mCurrentText + "_");
  return State::HANDLED_INPUT;
}

void Console::draw(float) {
  if (!isVisible())
    return;

  mProgram->bind();
  mRect->draw();
  mText->draw();
}