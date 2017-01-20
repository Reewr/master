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

  // Typedefs a handler as a function that takes in an asset,
  // which has access to a lot of stuff, followed by a string that is the input.
  //
  // The input is whatever is put in between the brackets.
  //
  // The return type is a string. If the string is not empty, it is
  // considered to be the output that will be shown to the user
  //
  // Should you need to display an error, throw an std::invalid_argument
  // error. This will displayed to the console.
  using Handler = std::function<std::string(Asset*              asset,
                                            const Input::Event& event,
                                            const std::string&  input)>;

  //! Create console
  Console(Asset* asset);
  ~Console();

  //! Adds a command to the console that can be invoked
  //! using the name followed by ().
  //! Ex: config.res(1024, 768)
  void addCommand(const std::string& name, Handler h);

  //! Sets an error on the console
  void error(const std::string& message);

  //! Logs an error to the console
  void log(const std::string &message);

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

private:
  // Checks if the command is legal, prints error if not.
  void doCommand(const Input::Event& event);

  // Internal to add text
  void addHistory(Text* text);

  // Sets the autocomplete by checking the current string
  void setAutoComplete();

  bool  mPrevInputOpened;

  std::string    mCurrentText;
  Text*          mText;
  Program*       mProgram;
  GL::Rectangle* mRect;
  GL::Rectangle* mAutoCompleteBox;
  Asset*         mAsset;

  bool               mShowAutoComplete;
  std::vector<Text*> mAutoComplete;
  std::vector<Text*> mHistory;

  std::map<std::string, Handler> mCommands;
};