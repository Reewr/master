#include "Asset.hpp"

#include "Utils.hpp"
#include "CFG.hpp"
#include "../Input/Input.hpp"
#include "../State/State.hpp"
#include "../Lua/Lua.hpp"

Asset::Asset(CFG* c) : mCFG(c), mInput(nullptr) {}

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

Lua::Lua* Asset::lua() {
  if (mLua == nullptr)
    throw Error("Tried to access Lua when nullptr");
  return mLua;
}

void Asset::setCFG(CFG* c) {
  mCFG = c;
}

void Asset::setInput(Input::Input* i) {
  mInput = i;
}

void Asset::setLua(Lua::Lua* l) {
  mLua = l;
}
