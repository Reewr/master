#pragma once

#include "../Log.hpp"
#include <vector>

class Asset;
class GUI;
class Drawable3D;

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
class State : public Logging::Log {
public:
  virtual ~State();
  virtual void update(float deltaTime)          = 0;
  virtual void input(const Input::Event& event) = 0;
  virtual void draw(float deltaTime)            = 0;

  Drawable3D* drawable(unsigned int index);
  unsigned int numDrawables();

  GUI* gui(unsigned int index);
  unsigned int numGUIs();

protected:
  State();

  float mDeltaTime;

  std::vector<Drawable3D*> mDrawable3D;
  std::vector<GUI*>        mGUIElements;
};
