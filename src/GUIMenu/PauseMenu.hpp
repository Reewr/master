#ifndef GUIMENU_PAUSEMENU_HPP
#define GUIMENU_PAUSEMENU_HPP

#include <string>

#include "../GUI/Window.hpp"
#include "../Math/Math.hpp"
#include "../Import/UILoader.hpp"

class Input;

class PauseMenu : public Window {
public:
  PauseMenu(Input* i);

  int handleKeyInput(const int key, const int action);
  int handleMouseButton(const int key, const int action);
  int handleAction();

private:
  Import::UILoader mUiLoader;
  Input* mInput;
};

#endif