#pragma once

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

  bool handleOptionsAction(const Input::Event& event);

  CFG*             mCFG;
  Input::Input*    mInput;
  Window*          mActiveWindow;
  Import::UILoader mUiLoader;
};