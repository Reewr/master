#pragma once

#include <string>

#include "../GUI/Window.hpp"
#include "../Import/UILoader.hpp"
#include "../Math/Math.hpp"

namespace Input {
  class Input;
  class Event;
}

class PauseMenu : public Window {
public:
  PauseMenu(Input::Input* i);

  void input(const Input::Event& event);

private:
  Import::UILoader mUiLoader;
  Input::Input*    mInput;
};