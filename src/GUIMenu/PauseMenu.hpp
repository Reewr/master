#pragma once

#include <string>

#include "../GUI/Window.hpp"

namespace Input {
  class Input;
  class Event;
}

class PauseMenu : public Window {
public:
  PauseMenu(Input::Input* i);

  void input(const Input::Event& event);

private:
  Input::Input* mInput;
};