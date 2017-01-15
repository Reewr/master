#pragma once

#include "../GUI/GUI.hpp"

class Text;

namespace Input {
class Input;
class Event;
}

namespace GL {
class Rectangle;
}

class Console : public GUI {
public:
  //! Create console
  Console(Input::Input* input);
  ~Console();

  void input(const Input::Event& event);
  //! Draw the console
  void draw(float dt);

private:
  std::string    mCurrentText;
  Text*          mText;
  Input::Input*  mInput;
  Program*       mProgram;
  GL::Rectangle* mRect;
};