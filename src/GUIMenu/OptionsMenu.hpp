#ifndef GUIMENU_OPTIONSMENU_HPP
#define GUIMENU_OPTIONSMENU_HPP

#include "../GUI/GUI.hpp"
#include "../GUI/Window.hpp"
#include "../Import/UILoader.hpp"

namespace Input {
class Input;
class Event;
}

class OptionsMenu : public Window {
public:
  OptionsMenu(Input::Input* i);

private:
  void setDefaultOptions();

  int parseOptionstoCFG();
  int parseGraphicsOptions();

  void parseGameOptions();
  void parseAudioOptions();
  void parseKeybindingOptions();

  bool handleAction(const Input::Event& event);

  Input::Input*    mInput;
  Window*          mActiveWindow;
  Import::UILoader mUiLoader;
};

#endif