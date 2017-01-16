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
  // considered to be an error string that will be shown to the user
  using Handler = std::function<std::string(Asset* asset, const std::string& input)>;

  //! Create console
  Console(Asset* asset);
  ~Console();

  void input(const Input::Event& event);
  //! Draw the console
  void draw(float dt);

private:
  std::string    mCurrentText;
  Text*          mText;
  Text*          mError;
  Program*       mProgram;
  GL::Rectangle* mRect;
  Asset*         mAsset;

  std::vector<Text*> mHistory;
  std::map<Command, Handler> mCommands;
};