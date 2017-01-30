#pragma once

#include <vector>

class Asset;
class GUI;
class Drawable;

namespace Input {
  class Event;
}

namespace States {
  enum {
    QuitAll = -2,
    Quit    = -1,
    Init,
    OptionsMenuClose,
    OptionsMenuOpen,
    PauseMenuOpen,
    PauseMenuClose,
    MainMenu,
    Game,
    MasterThesis,
    Refresh,
    WinRefresh,
    NoChange,
    LuaReload
  };
}

//! Fully Virtual class utilized by StateGame and StateMenu
class State {
public:
  virtual ~State();
  virtual void update(float deltaTime)          = 0;
  virtual void input(const Input::Event& event) = 0;
  virtual void draw(float deltaTime)            = 0;

protected:
  State();

  float mDeltaTime;

  std::vector<Drawable*> mDrawable;
  std::vector<GUI*>      mGUIElements;
};
