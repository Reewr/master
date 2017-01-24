#include "Console.hpp"

#include "../OpenGLHeaders.hpp"
#include <sol.hpp>

#include "../GLSL/Program.hpp"
#include "../GUI/Text.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Lua/Lua.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../State/State.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

using mmm::vec2;
using mmm::vec4;

Console::Console(Asset* asset)
    : mLocation(0)
    , mCurrentText("")
    , mAutoCompleteBox(new GLRectangle(Rectangle(0, 0, 0, 0)))
    , mAsset(asset)
    , mShowAutoComplete(false) {
  vec2 res     = asset->cfg()->graphics.res;
  vec2 textPos = vec2(10, res.y / 2 - 30);
  mBoundingBox = Rectangle(0, 0, res.x, res.y / 2);

  mRect = new GLRectangle(mBoundingBox);
  mText = new Text("Font::Dejavu",
                   "> _",
                   textPos,
                   15,
                   Text::WHITE,
                   vec2(mAsset->cfg()->graphics.res.x,
                        mAsset->cfg()->graphics.res.y));
  mText->isVisible(true);
  ::log("res: ", res, " textPos: ", textPos, " ");

  // Specific program for the console since the console
  // is just drawn in black with alpha
  mProgram = mAsset->rManager()->get<Program>("Program::ColorRect");
  mProgram->setUniform("screenRes", res);
  mProgram->setUniform("guiColor", vec4(0, 0, 0, 0.9));
}

Console::~Console() {
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
  addHistory(message, Text::RED);
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
  addHistory(message, Text::YELLOW);
}

/**
 * @brief
 *   Logs a message to the console. The message is white.
 *
 * @param message
 */
void Console::log(const std::string& message) {
  ::log("[Console] ", message);
  addHistory(message, Text::WHITE);
}

/**
 * @brief
 *   Just like the other addHistory() function, but this creates the
 *   text element before calling the other addHistory
 *
 * @param message
 * @param color
 */
void Console::addHistory(const std::string& message, int color) {
  CFG* cfg   = mAsset->cfg();
  vec2 limit = vec2(cfg->graphics.res.x, cfg->graphics.res.y);
  addHistory(new Text("Font::Dejavu", message, vec2(0, 0), 12, color, limit));
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

    const Rectangle& box = (*a)->box();
    startPos             = startPos - vec2(0, box.size.y + 5);

    (*a)->setOffset(startPos);
  }
}

void Console::addCharacter(const std::string& c) {
  mCurrentText.insert(mLocation, c);
  mLocation += c.size();
  setText();
}

/**
 * @brief
 *   Tries to run the current inputted string in Lua, printing
 *   any result / erorrs to the console
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
  mLocation    = 0;
  setText();
}

/**
 * @brief
 *   Internal version of setText("") that sets the text
 *   to the internal stored value.
 */
void Console::setText() {
  int size = mCurrentText.size();

  if (mLocation > size)
    mLocation = size;

  if (mLocation < 0)
    mLocation = 0;

  if (mLocation == size)
    return mText->setText("> " + mCurrentText + "_");

  std::string before = mCurrentText.substr(0, mLocation);
  std::string after  = mCurrentText.substr(mLocation);

  mText->setText("> " + before + "_" + after);
}

/**
 * @brief
 *   Checks the current text to see if there are any commands
 *   that contain the words in them. If so, display those
 *   commands.
 *
 *   Also adjusts the autocompletion box to match width
 *   and height of the texts.
 *
 *   @TODO: Make this work with Lua
 */
void Console::setAutoComplete() {
  return;

  /* vec2      res      = mAsset->cfg()->graphics.res; */
  /* Rectangle box      = Rectangle(5, res.y / 2 + 30, 0, 0); */
  /* int       index    = 0; */
  /* vec2      startPos = vec2(10, res.y / 2 + 30); */

  /* for (auto a : mAutoComplete) { */
  /*   bool hasText = a->getText().find(mCurrentText) != std::string::npos; */
  /*   if (hasText) { */
  /*     a->isVisible(true); */
  /*     a->setOffset(startPos + vec2(0, 20 * index)); */
  /*     box.size.x = max(a->size().x, box.size.x); */
  /*     box.size.y = max(a->size().y, box.size.y); */
  /*     index++; */
  /*   } else { */
  /*     a->isVisible(false); */
  /*   } */
  /* } */

  /* if (box.size.y > 0 && index != 0) { */
  /*   box.size.x        = box.size.x + 10; */
  /*   mShowAutoComplete = true; */
  /*   mAutoCompleteBox->change(box); */
  /* } else { */
  /*   mShowAutoComplete = false; */
  /* } */
}

/**
 * @brief
 *   Handles the key input for a console
 *
 * @return
 */
void Console::input(const Input::Event& event) {
  if (!isVisible() && event.isAction(Input::Action::Console)) {
    mPrevInputOpened = true;
    isVisible(true);
    return event.stopPropgation();
  }

  // Ignore all other events
  bool acceptedEvent = event == Input::Event::Type::KeyPress ||
                       event == Input::Event::Type::CharacterInput ||
                       event == Input::Event::Type::KeyRepeat;
  if (!isVisible() || !acceptedEvent) {
    return;
  }

  if (event == Input::Event::Type::KeyPress) {
    switch (event.key()) {
      // Escape means close console
      case GLFW_KEY_ESCAPE:
        isVisible(false);
        return event.stopPropgation();

      // Enter means do command
      case GLFW_KEY_ENTER:
        doCommand(event);
        return event.stopPropgation();

      case GLFW_KEY_A:
        if (event.hasCtrl()) {
          mLocation = 0;
          setText();
          return event.stopPropgation();
        }
        break;

      case GLFW_KEY_E:
        if (event.hasCtrl()) {
          mLocation = mCurrentText.size();
          setText();
          return event.stopPropgation();
        }
        break;
    }
  }

  if (event == Input::Event::Type::CharacterInput && !mPrevInputOpened) {
    addCharacter(event.character());
    return event.stopPropgation();
  } else if (event == Input::Event::Type::CharacterInput) {
    mPrevInputOpened = false;
    return;
  }

  // If backspace is held down, delete one character
  // at the time, so keypress and keyrelease are not
  // handled as two seperate events.
  bool isBackspace = event.key() == GLFW_KEY_BACKSPACE;
  bool isDelete    = event.key() == GLFW_KEY_DELETE;

  if (isBackspace || isDelete) {
    int key = isBackspace ? GLFW_KEY_BACKSPACE : GLFW_KEY_DELETE;

    if (event.hasAlt())
      deleteWord(key);
    else
      deleteCharacter(key);

    return event.stopPropgation();
  }

  // If left arrow is held down, move left in the console
  if (event.key() == GLFW_KEY_LEFT) {
    mLocation = mLocation - 1;
    setText();
    return event.stopPropgation();
  }

  // if right arrow is held down, move right
  if (event.key() == GLFW_KEY_RIGHT) {
    mLocation = mLocation + 1;
    setText();
    return event.stopPropgation();
  }
}

/**
 * @brief
 *   Deletes a character using either backspace or delete.
 *   Depending on which, it will try to delete either the character
 *   behind the current position or infront of it
 *
 * @param whichKey
 */
void Console::deleteCharacter(int whichKey) {
  int size = mCurrentText.size();

  if (whichKey == GLFW_KEY_DELETE) {
    if (mLocation == size)
      return;

    mCurrentText.erase(mLocation, 1);
    setText();
    return;
  }

  if (whichKey == GLFW_KEY_BACKSPACE) {
    if (mLocation == 0)
      return;

    mCurrentText.erase(mLocation - 1, 1);
    setText();
    return;
  }
}

static std::string NON_SEPERATORS =
  "abcdefghijklmnopqrtstuvwxyzABCDEFGHIJKLMNOPQRTSTUVWXYZ";

void Console::deleteWord(int whichKey) {
  int size = mCurrentText.size();

  if (whichKey == GLFW_KEY_DELETE) {
    if (mLocation == size)
      return;

    size_t pos = mCurrentText.find_first_not_of(NON_SEPERATORS, mLocation);

    if (pos == std::string::npos) {
      mCurrentText = mCurrentText.substr(0, mLocation);
    }
    {
      mCurrentText =
        mCurrentText.substr(0, mLocation) + mCurrentText.substr(pos);
    }

    setText();
    return;
  }

  if (whichKey == GLFW_KEY_BACKSPACE) {
    if (mLocation == 0)
      return;

    size_t pos = mCurrentText.find_last_not_of(NON_SEPERATORS, mLocation);

    if (pos == std::string::npos)
      mCurrentText = mCurrentText.substr(mLocation);
    else {
      mCurrentText =
        mCurrentText.substr(0, pos) + mCurrentText.substr(mLocation);
    }

    setText();
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
void Console::update(float) {}

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