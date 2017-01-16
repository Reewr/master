#include "Asset.hpp"
#include "Utils.hpp"

Asset::Asset(CFG* c)
    : mCFG(c)
    , mInput(nullptr)
    , mEngine(nullptr)
    , mState(nullptr) {}

Asset::Asset(CFG* c, Input::Input* i, Engine* e, State* s)
    : mCFG(c)
    , mInput(i)
    , mEngine(e)
    , mState(s) {}

CFG* Asset::cfg() {
  if (mCFG == nullptr)
    throw Error("Tried to access CFG when nullptr");
  return mCFG;
}

Input::Input* Asset::input() {
  if (mInput == nullptr)
    throw Error("Tried to access Input when nullptr");
  return mInput;
}

Engine* Asset::engine() {
  if (mEngine == nullptr)
    throw Error("Tried to access Engine when nullptr");
  return mEngine;
}

State* Asset::state() {
  if (mState == nullptr)
    throw Error("Tried to access State when nullptr");
  return mState;
}

void Asset::setCFG(CFG* c) {
  mCFG = c;
}

void Asset::setInput(Input::Input* i) {
  mInput = i;
}

void Asset::setEngine(Engine* e) {
  mEngine = e;
}

void Asset::setState(State* s) {
  mState = s;
}
