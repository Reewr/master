#include "Console.hpp"

#include "../OpenGLHeaders.hpp"

#include "../GLSL/Program.hpp"
#include "../GUI/Menu.hpp"
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
    : Logging::Log("Console")
    , mShowAutoComplete(false)
    , mLocation(0)
    , mCommandHistoryIndex(0)
    , mCurrentText("")
    , mAutoCompleteBox(new GLRectangle(Rectangle(0, 0, 0, 0)))
    , mAsset(asset)
    , mLuaAutoComplete({}) {
  CFG* cfg     = asset->cfg();
  vec2 res     = cfg->graphics.res;
  vec2 textPos = vec2(10, res.y / 2 - 30);
  mBoundingBox = Rectangle(0, 0, res.x, res.y / 2);

  mRect = new GLRectangle(mBoundingBox);
  mText = new Text("Font::Dejavu",
                   "> _",
                   textPos,
                   15,
                   Text::WHITE,
                   cfg->graphics.res);

  mAutoComplete = new Menu();
  mText->isVisible(true);

  // Specific program for the console since the console
  // is just drawn in black with alpha
  mProgram = mAsset->rManager()->get<Program>("Program::ColorRect");
  mProgram->setUniform("guiColor", vec4(0, 0, 0, 0.9));

  mAsset->lua()->add(this);
  mAsset->lua()->loadFile("lua/console/main.lua");
}

Console::~Console() {
  delete mRect;
  delete mText;
  delete mAutoComplete;
  delete mAutoCompleteBox;

  for (auto h : mHistory)
    delete h;

  mHistory.clear();
  mCommandHistory.clear();
  mLuaAutoComplete.clear();
}

/**
 * @brief
 *   Logs an error to the console. The error is red.
 *
 * @param message
 *   The message to display
 */
void Console::error(const std::string& message) {
  mLog->error(Text::stripColorsFromStr(message));
  addHistory(message, Text::RED);
}

/**
 * @brief
 *   Logs a message to the console. The message is white.
 *
 * @param message
 */
void Console::log(const std::string& message) {
  mLog->debug(Text::stripColorsFromStr(message));
  addHistory(message, Text::WHITE);
}

/**
 * @brief
 *   Logs an error to the console. The error is red.
 *
 * @param message
 *   The message to display
 */
void Console::warn(const std::string& message) {
  mLog->warn(Text::stripColorsFromStr(message));
  addHistory(message, Text::YELLOW);
}

/**
 * @brief
 *   Clears the console of text
 */
void Console::clear() {
  for (auto h : mHistory)
    delete h;

  mHistory.clear();
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
 *   Sets the current text to be a text retrieved from the command
 *   history. It will make sure that the command history index
 *   does not go over any limits.
 *
 */
void Console::setCommandFromHistory() {
  int commandHistoryLen = mCommandHistory.size();

  if (mCommandHistoryIndex < 0)
    mCommandHistoryIndex = 0;

  if (mCommandHistoryIndex >= commandHistoryLen) {
    mCommandHistoryIndex = commandHistoryLen;
    return setText("");
  }

  // retrieve the command strip the '> ' characters
  setText(mCommandHistory[mCommandHistoryIndex]);
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
    mCommandHistory.push_back(mCurrentText);
    mCommandHistoryIndex                  = mCommandHistory.size();
    mAsset->lua()->engine["currentEvent"] = &event;
    mAsset->lua()->engine.script(mCurrentText);
  } catch (const sol::error& e) {
    error("> " + mCurrentText + " - Failed");
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
  mLocation    = s.size();
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
    return mText->setText("> " + mCurrentText +
                          "\\<0,0,0,255:255,255,255,125> \\</>");

  std::string before    = mCurrentText.substr(0, mLocation);
  std::string character = mCurrentText.substr(mLocation, 1);
  std::string after     = mCurrentText.substr(mLocation + 1);

  mText->setText("> " + before + "\\<0,0,0,255:255,255,255,125>" + character +
                 "\\</>" + after);
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
  std::vector<Lua::TypePair> names = mAsset->lua()->getTypenames(mCurrentText);

  vec2         res        = mAsset->cfg()->graphics.res;
  unsigned int maxNameLen = 0;
  unsigned int maxTypeLen = 0;
  int          namesLen   = names.size();
  bool         isEqual    = (int) mLuaAutoComplete.size() == namesLen;

  // find the max length of name and type so that they
  // can be aligned properly
  for (int i = 0; i < namesLen; i++) {
    isEqual    = isEqual && names[i] == mLuaAutoComplete[i];
    maxNameLen = mmm::max(names[i].first.size(), maxNameLen);
    maxTypeLen = mmm::max(names[i].second.size(), maxTypeLen);
  }

  if (isEqual) {
    mAutoComplete->setActiveMenu(mAutoComplete->getActiveMenu() + 1);
    return;
  }

  mLuaAutoComplete = names;
  mAutoComplete->clearMenuItems();
  mAutoComplete->setActiveMenu(-1);

  Menu::MenuSettings       settings(12, 10, Menu::VERTICAL, Text::WHITE);
  vec2                     startPos = vec2(10, res.y / 2);
  std::vector<std::string> row      = {};

  for (int i = 0; i < namesLen; ++i) {
    std::string name    = names[i].first;
    std::string type    = names[i].second;
    std::string content = "";

    if (name.size() < maxNameLen)
      name.append(maxNameLen - name.size(), ' ');

    if (type.size() < maxTypeLen)
      type.append(maxTypeLen - type.size(), ' ');

    content += name + " :: " + type;

    if (i + 13 < namesLen)
      content += " | ";

    row.push_back(content);

    if ((i % 14 == 0 && i != 0) || i + 1 == namesLen) {
      mAutoComplete->addMenuItems(row, startPos, settings);
      row.clear();
      startPos.x = mAutoComplete->size().x + 10;
    }
  }

  mLuaAutoComplete = names;

  if (names.size() == 0) {
    mShowAutoComplete = false;
    return;
  }

  const vec2& size = mAutoComplete->size();
  Rectangle   box  = Rectangle(0, res.y / 2, size.x + 20, size.y + 10);
  mAutoCompleteBox->change(box);
  mShowAutoComplete = true;
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
      case GLFW_KEY_TAB:
        setAutoComplete();
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
    --mLocation;
    setText();
    return event.stopPropgation();
  }

  // if right arrow is held down, move right
  if (event.key() == GLFW_KEY_RIGHT) {
    ++mLocation;
    setText();
    return event.stopPropgation();
  }

  // if up arrow is held down, go up through the history
  if (event.key() == GLFW_KEY_DOWN) {
    ++mCommandHistoryIndex;
    setCommandFromHistory();
    return event.stopPropgation();
  }

  // if down arrow is held down, go down through the history
  if (event.key() == GLFW_KEY_UP) {
    --mCommandHistoryIndex;
    setCommandFromHistory();
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
    --mLocation;
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
    mAutoComplete->draw();
  }

  mText->draw();

  for (auto a : mHistory) {
    a->draw();
  }
}
