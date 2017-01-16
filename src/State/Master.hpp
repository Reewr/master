#pragma once

#include "State.hpp"

namespace Input {
class Input;
class Event;
}
class Console;
class Asset;

class Master : public State {
public:
  Master(Asset* asset);

  ~Master();

  void update(float deltaTime);

  void draw(float deltaTime);

  void input(const Input::Event& event);

private:
  void draw3D();
  void drawGUI();

  /* Input::Input* mInput; */
  /* Asset*        mAsset; */
};
