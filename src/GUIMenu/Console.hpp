#ifndef GUIMENU_CONSOLE_HPP
#define GUIMENU_CONSOLE_CPP

#include "../GUI/GUI.hpp"

class Text;
class Input;

namespace GL {
class Rectangle;
}

class GConsole : public GUI {
public:
  //! Create console
  GConsole(Input* input);
  ~GConsole();

  //! Handles the keys sent to console
  int handleKeyInput(const int key, const int action, const int mods);

  //! Draw the console
  void draw(float dt);

private:
  std::string    mCurrentText;
  Text*          mText;
  Input*         mInput;
  Program*       mProgram;
  GL::Rectangle* mRect;
};

#endif