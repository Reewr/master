#pragma once

#include "CFG.hpp"
#include "../Engine.hpp"
#include "../State/State.hpp"
#include "../Input/Input.hpp"

//! Asset is a class that is being sent around that stores a lot of useful
//! settings.
class Asset {
public:
  Asset(CFG* c);
  Asset(CFG* c, Input::Input* i, Engine* e, State* s);

  CFG* cfg();
  Input::Input* input();
  Engine* engine();
  State* state();

  void setCFG(CFG* c);
  void setInput(Input::Input* i);
  void setEngine(Engine* e);
  void setState(State* s);

private:
  CFG* mCFG;
  Input::Input* mInput;
  Engine* mEngine;
  State* mState;
};