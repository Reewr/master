#pragma once

#include "State.hpp"

namespace Input {
class Input;
class Event;
}
class Console;
struct Asset;

class Master : public State {
public:
  Master(Asset* asset, Input::Input* input);

  ~Master();

  void update(float deltaTime);

  void input(const Input::Event& event);

private:
  void draw3D();
  void drawGUI();

  Input::Input* mInput;
  Asset*        mAsset;
  Console*      mConsole;
};
