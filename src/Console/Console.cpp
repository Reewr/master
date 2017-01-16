#include "Console.hpp"

#include "../OpenGLHeaders.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Math/Math.hpp"
#include "../State/State.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/Asset.hpp"

Console::Console(Asset* asset) {
  vec2 res = asset->cfg()->graphics.res;
  mAsset = asset;
  mCurrentText = "";
  mBoundingBox = Rect(0, 0, res.x, res.y / 2);
  mRect        = new GL::Rectangle(mBoundingBox);
  mText        = new Text(mFont, "> _", vec2(10, res.y / 2), 20, Text::WHITE);

  // Specific program for the console since the console
  // is just drawn in black with alpha
  mProgram = new Program("shaders/GUI/ColorRect.vsfs", 0);
  mProgram->bindAttribs({ "position", "texcoord" }, { 0, 1 });
  mProgram->link();
  mProgram->setUniform("screenRes", res, "guiOffset", vec2());
  mProgram->setUniform("guiColor", vec4(0, 0, 0, 0.7));
}

Console::~Console() {
  delete mProgram;
  delete mRect;
  delete mText;
  delete mError;

  for (auto a : mHistory)
    delete a;
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
void Console::input(const Input::Event& event) {
  bool isConsoleKey = mAsset->input()->checkKey(Input::Action::Console, event.key());

  if (!isVisible() && isConsoleKey) {
    isVisible(true);
    return event.stopPropgation();
  }

  if (!isVisible()) {
    return;
  }

  if (event.keyPressed(GLFW_KEY_ESCAPE)) {
    isVisible(false);
    return event.stopPropgation();
  }

  if (event.key() == GLFW_KEY_BACKSPACE) {
    if (mCurrentText.length() > 0)
      mCurrentText.pop_back();

    mText->setText("> " + mCurrentText + "_");
    return event.stopPropgation();
  }

  if (event.keyPressed(GLFW_KEY_ENTER)) {
    log("Info should perform action with '" + mCurrentText + "'");
    return event.stopPropgation();
  }

  if (event == Input::Event::Type::CharacterInput) {
    mCurrentText.append(event.character());
    mText->setText("> " + mCurrentText + "_");
    return event.stopPropgation();
  }
}

void Console::draw(float) {
  if (!isVisible())
    return;

  mProgram->bind();
  mRect->draw();
  mText->draw();
}