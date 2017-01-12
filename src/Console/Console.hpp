#ifndef CONSOLE_CONSOLE_HPP
#define CONSOLE_CONSOLE_CPP

#include "../GUI/GUI.hpp"

class Text;

namespace Input {
class Input;
}

namespace GL {
class Rectangle;
}

class Console : public GUI {
public:
  //! Create console
  Console(Input::Input* input);
  ~Console();

  //! Handles the keys sent to console
  int handleKeyInput(const int key, const int action, const int mods);

  //! Draw the console
  void draw(float dt);

private:
  std::string    mCurrentText;
  Text*          mText;
  Input::Input*  mInput;
  Program*       mProgram;
  GL::Rectangle* mRect;
};

#endif