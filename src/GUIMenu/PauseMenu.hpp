#ifndef GUIMENU_PAUSEMENU_HPP
#define GUIMENU_PAUSEMENU_HPP

#include <string>

#include "../GUI/Window.hpp"
#include "../Import/UILoader.hpp"
#include "../Math/Math.hpp"

namespace Input {
class Input;
}

class PauseMenu : public Window {
public:
  PauseMenu(Input::Input* i);

  int handleKeyInput(const int key, const int action);
  int handleMouseButton(const int key, const int action);
  int handleAction();

private:
  Import::UILoader mUiLoader;
  Input::Input*    mInput;
};

#endif