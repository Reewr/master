#pragma once

#include "../GUI/GUI.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

class Text;
class CFG;
class Asset;


namespace Input {
  class Input;
  class Event;
}

namespace GL {
  class Rectangle;
}

class Console : public GUI {
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


  // Checks if the command is legal, prints error if not.
  void doCommand(const Input::Event& event);

  // Internal to add text
  void addHistory(Text* text);

  // Sets the autocomplete by checking the current string
  void setAutoComplete();

  void deleteCharacter(int whichKey);

  bool mPrevInputOpened;

  // Stores the location of the pointer
  int mLocation;

  // Stores the text that the user inputs (command)
  std::string    mCurrentText;

  // This holds the text that is rendered on the screen
  Text*          mText;
  GL::Rectangle* mRect;
  GL::Rectangle* mAutoCompleteBox;

  Program*       mProgram;
  Asset*         mAsset;

  bool               mShowAutoComplete;
  std::vector<Text*> mAutoComplete;
  std::vector<Text*> mHistory;
};