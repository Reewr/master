#pragma once

#include <vector>

class Asset;
class GUI;

namespace Input {
class Event;
}

//! Fully Virtual class utilized by StateGame and StateMenu
class State {
public:
  enum {
    QUITALL = -2,
    QUIT    = -1,
    INIT,
    MAINMENU,
    GAME,
    MASTER_THESIS,
    REFRESH,
    WINREFRESH,
    NOCHANGE,
    HANDLED_INPUT
  };

  virtual ~State();
  virtual void update(float deltaTime)          = 0;
  virtual void input(const Input::Event& event) = 0;

protected:
  State();

  virtual void draw3D()  = 0;
  virtual void drawGUI() = 0;

  float             mDeltaTime;
  std::vector<GUI*> mGUIElements;
};
