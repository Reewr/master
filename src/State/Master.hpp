#pragma once

#include "State.hpp"

namespace Input {
  class Input;
  class Event;
}

namespace Lua {
  class Lua;
}

class Console;
class Asset;
class Camera;
class Framebuffer;
class World;
class SpiderSwarm;

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

  Camera*      mCamera;
  Framebuffer* mShadowmap;
  World*       mWorld;
  Lua::Lua*    mLua;
  SpiderSwarm* mSwarm;
  // Input::Input* mInput;
  Asset* mAsset;
};
