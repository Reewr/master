#pragma once

#include <vector>

class Asset;
class GUI;

namespace Input {
class Event;
}

namespace States {
  enum {
    QuitAll = -2,
    Quit    = -1,
    Init,
    OptionsMenu,
    PauseMenu,
    Console,
    MainMenu,
    Game,
    MasterThesis,
    Refresh,
    WinRefresh,
    NoChange
  };
}

//! Fully Virtual class utilized by StateGame and StateMenu
class State {
public:

  virtual ~State();
  virtual void update(float deltaTime)          = 0;
  virtual void input(const Input::Event& event) = 0;
  virtual bool isTransparent() = 0;
  virtual void draw(float deltaTime) = 0;

protected:
  State();

  bool              mTransparent;
  float             mDeltaTime;
  std::vector<GUI*> mGUIElements;
};
