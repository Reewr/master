#ifndef GUIMENU_OPTIONSMENU_HPP
#define GUIMENU_OPTIONSMENU_HPP

#include "../GUI/Window.hpp"
#include "../Import/UILoader.hpp"

namespace Input {
class Input;
}

class OptionsMenu : public Window {
public:
  OptionsMenu(Input::Input* i);
  bool hasChanged();

  void hasChanged(bool c);

  int handleKeyInput(const int key, const int action);
  int handleMouseButton(const int key, const int action);
  void handleMouseMovement(const vec2& pos);

private:
  void setDefaultOptions();

  int parseOptionstoCFG();
  int parseGraphicsOptions();

  void parseGameOptions();
  void parseAudioOptions();
  void parseKeybindingOptions();

  int handleAction();

  Input::Input*    mInput;
  Window*          mActiveWindow;
  Import::UILoader mUiLoader;
};

#endif