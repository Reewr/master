#include "Console.hpp"

#include "../OpenGLHeaders.hpp"
#include <sol.hpp>

#include "../GLSL/Program.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Lua/Lua.hpp"
#include "../Math/Math.hpp"
#include "../State/State.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

Console::Console(Asset* asset)
    : mLocation(0)
    , mCurrentText("")
    , mAutoCompleteBox(new GL::Rectangle(Rect(0, 0, 0, 0)))
    , mAsset(asset)
    , mShowAutoComplete(false) {
  vec2 res     = asset->cfg()->graphics.res;
  vec2 textPos = vec2(10, res.y / 2 - 30);
  mBoundingBox = Rect(0, 0, res.x, res.y / 2);

  mRect = new GL::Rectangle(mBoundingBox);
  mText = new Text(mFont,
                   "> _",
                   textPos,
                   12,
                   Text::WHITE,
                   vec2(mAsset->cfg()->graphics.res.x,
                        mAsset->cfg()->graphics.res.y));

  // Specific program for the console since the console
  // is just drawn in black with alpha
  mProgram = new Program("shaders/GUI/ColorRect.vsfs", 0);
  mProgram->bindAttribs({ "position", "texcoord" }, { 0, 1 });
  mProgram->link();
  mProgram->setUniform("screenRes", res, "guiOffset", vec2());
  mProgram->setUniform("guiColor", vec4(0, 0, 0, 0.9));
}

Console::~Console() {
  delete mProgram;
  delete mRect;
  delete mText;
  delete mAutoCompleteBox;

  for (auto a : mAutoComplete)
    delete a;

  for (auto h : mHistory)
    delete h;

  mAutoComplete.clear();
  mHistory.clear();
}

/**
 * @brief
 *   Logs an error to the console. The error is red.
 *
 * @param message
 *   The message to display
 */
void Console::error(const std::string& message) {
  ::error("[Console] ", message);
  addHistory(new Text(mFont,
                      message,
                      vec2(0, 0),
                      12,
                      Text::RED,
                      vec2(mAsset->cfg()->graphics.res.x,
                           mAsset->cfg()->graphics.res.y)));
}

/**
 * @brief
 *   Logs an error to the console. The error is red.
 *
 * @param message
 *   The message to display
 */
void Console::warn(const std::string& message) {
  ::warning("[Console] ", message);
  addHistory(new Text(mFont,
                      message,
                      vec2(0, 0),
                      12,
                      Text::YELLOW,
                      vec2(mAsset->cfg()->graphics.res.x,
                           mAsset->cfg()->graphics.res.y)));
}

/**
 * @brief
 *   Logs a message to the console. The message is white.
 *
 * @param message
 */
void Console::log(const std::string& message) {
  ::log("[Console] ", message);
  addHistory(new Text(mFont,
                      message,
                      vec2(0, 0),
                      12,
                      Text::WHITE,
                      vec2(mAsset->cfg()->graphics.res.x,
                           mAsset->cfg()->graphics.res.y)));
}

/**
 * @brief
 *   When a command has been executed, this function
 *   is called which adds the current text to a list of
 *   history as a new Text element.
 *
 *   This is displayed on the screen until it goes outside it.
 *   A text element will gradiually be moved upwards until
 *   it is outside of the screen, at which point it will be hidden
 */
void Console::addHistory(Text* text) {
  vec2 startPos = vec2(10, mAsset->cfg()->graphics.res.y / 2 - 30);

  text->isVisible(true);
  mHistory.push_back(text);

  for (auto a = mHistory.rbegin(); a < mHistory.rend(); a++) {
    if (startPos.y < 0) {
      (*a)->isVisible(false);
      continue;
    }

    const Rect& box = (*a)->box();
    startPos        = startPos - vec2(0, box.size.y + 5);

    (*a)->setOffset(startPos);
  }
}

/**
 * @brief
 *   Checks the current text for errors, if any, prints error.
 *   If there are no errors and the text matches a command
 *   it will call that command with the text within the brackets.
 *
 *   Good commands:
 *
 *     config.res(100, 100))
 *     config.res.x(100)
 *     config.res.y(100)
 *
 *   Bad commands:
 *     config.res = 100
 *     config.res(10
 *     config.res)(
 *
 * @param event
 */
void Console::doCommand(const Input::Event& event) {
  try {
    log("> " + mCurrentText);
    mAsset->lua()->engine["currentEvent"] = &event;
    mAsset->lua()->engine.script(mCurrentText);
  } catch (const sol::error& e) {
    error(e.what());
  }

  setText("");
}

/**
 * @brief
 *   Sets the text of the console, also starts
 *   the autocomplete.
 *
 * @param s
 */
void Console::setText(const std::string& s) {
  mCurrentText = s;
  mText->setText("> " + mCurrentText + "_");
}

/**
 * @brief
 *   Checks the current text to see if there are any commands
 *   that contain the words in them. If so, display those
 *   commands.
 *
 *   Also adjusts the autocompletion box to match width
 *   and height of the texts.
 */
void Console::setAutoComplete() {
  return;

  vec2 res      = mAsset->cfg()->graphics.res;
  Rect box      = Rect(5, res.y / 2 + 30, 0, 0);
  int  index    = 0;
  vec2 startPos = vec2(10, res.y / 2 + 30);

  for (auto a : mAutoComplete) {
    bool hasText = a->getText().find(mCurrentText) != std::string::npos;
    if (hasText) {
      a->isVisible(true);
      a->setOffset(startPos + vec2(0, 20 * index));
      box.size.x = max(a->size().x, box.size.x);
      box.size.y = max(a->size().y, box.size.y);
      index++;
    } else {
      a->isVisible(false);
    }
  }

  if (box.size.y > 0 && index != 0) {
    box.size.x        = box.size.x + 10;
    mShowAutoComplete = true;
    mAutoCompleteBox->change(box);
  } else {
    mShowAutoComplete = false;
  }
}

/**
 * @brief
 *   Handles the key input for a console
 *
 * @return
 */
void Console::input(const Input::Event& event) {
  bool isConsoleKey =
    mAsset->input()->checkKey(Input::Action::Console, event.key());

  if (!isVisible() && isConsoleKey) {
    mPrevInputOpened = true;
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

  if (event.isKeyHeldDown(GLFW_KEY_BACKSPACE)) {
    if (mCurrentText.length() > 0)
      mCurrentText.pop_back();

    setText(mCurrentText);
    return event.stopPropgation();
  }

  if (event.keyPressed(GLFW_KEY_ENTER)) {
    doCommand(event);
    return event.stopPropgation();
  }

  if (event == Input::Event::Type::CharacterInput && !mPrevInputOpened) {
    setText(mCurrentText + event.character());
    return event.stopPropgation();
  } else if (event == Input::Event::Type::CharacterInput) {
    mPrevInputOpened = false;
  }
}

/**
 * @brief
 *   Updates the console. This only does one thing
 *   at the moment which is count down the time
 *   it should display the error, if it is displayed
 *
 * @param deltaTime
 */
void Console::update(float deltaTime) {}

/**
 * @brief Draw!
 */
void Console::draw() {
  if (!isVisible())
    return;

  mProgram->bind();
  mRect->draw();

  if (mShowAutoComplete) {
    mAutoCompleteBox->draw();
    for (auto a : mAutoComplete)
      a->draw();
  }

  mText->draw();

  for (auto a : mHistory) {
    a->draw();
  }
}