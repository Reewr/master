#pragma once

#include "../Engine.hpp"
#include "../Input/Input.hpp"
#include "../State/State.hpp"
#include "CFG.hpp"

//! Asset is a class that is being sent around that stores a lot of useful
//! settings.
class Asset {
public:
  Asset(CFG* c);

  CFG*          cfg();
  Input::Input* input();

  void setCFG(CFG* c);
  void setInput(Input::Input* i);

private:
  CFG*          mCFG;
  Input::Input* mInput;
};