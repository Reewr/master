#pragma once

#include "../GUI/GUI.hpp"
#include "../Log.hpp"

#include <map>
#include <string>
#include <vector>

class Text;
class CFG;
class Asset;
class Menu;

namespace Lua {
  typedef std::pair<std::string, std::string> TypePair;
}

namespace Input {
  class Input;
  class Event;
}

class GLRectangle;

/**
 * @brief
 *   This represents a Console that can be used together with Lua
 *   to interact with the engine itself.
 *
 *   \todo:
 *     - Add tab completion
 *     - Add scroll in history
 *     - Add movement by word, left and right
 *     - Fix lua to output to console
 *     - History size in config
 *     - Lua help (help someFunction to indication of arguments)
 *       - help help!
 *     - Add aliases (quit, :q, exit)
 */
class Console : public GUI, public Logging::Log {
public:
  using Command = std::string;

  //! Create console
  Console(Asset* asset);
  ~Console();

  //! Sets an error on the console
  void error(const std::string& message);

  //! Logs an error to the console
  void log(const std::string& message);

  //! Logs a yellow warning to the console
  void warn(const std::string& message);

  //! Clears the history of the console
  void clear();

  // Input handler
  void input(const Input::Event& event);

  // Sets the text of the console
  void setText(const std::string& s);

  // Update handler
  void update(float dt);

  //! Draw the console
  void draw();

  // Add a character to where the cursor is currently at
  void addCharacter(const std::string& c);

private:
  // Internal set text, which is used whenever the text should be
  // set to the stored text in mCurrentText
  void setText();

  // Sets the command from history based on mCommandHistoryIndex
  void setCommandFromHistory();

  // Checks if the command is legal, prints error if not.
  void doCommand(const Input::Event& event);

  // Adds history, shortcut to making a text of font size 12
  void addHistory(const std::string& message, int color);

  // Internal to add text
  void addHistory(Text* text);

  // Sets the autocomplete by checking the current string
  void setAutoComplete();

  // deletes one character either infront or behind current
  // position based on the key pressed
  void deleteCharacter(int whichKey);

  // deletes one word either infront or behind the current
  // position based on the key pressed
  void deleteWord(int whichKey);

  bool mShowAutoComplete;
  bool mPrevInputOpened;

  // Stores the location of the pointer
  int mLocation;
  int mCommandHistoryIndex;

  // Stores the text that the user inputs (command)
  std::string mCurrentText;

  // This holds the text that is rendered on the screen
  Text*        mText;
  Menu*        mAutoComplete;
  GLRectangle* mRect;
  GLRectangle* mAutoCompleteBox;

  std::shared_ptr<Program> mProgram;
  Asset*                   mAsset;

  std::vector<Text*>         mHistory;
  std::vector<std::string>   mCommandHistory;
  std::vector<Lua::TypePair> mLuaAutoComplete;
};
