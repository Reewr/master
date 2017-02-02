#include "Asset.hpp"

#include "../Input/Input.hpp"
#include "../Lua/Lua.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../State/State.hpp"
#include "CFG.hpp"
#include "Utils.hpp"

Asset::Asset(CFG* c) : mCFG(c) {}

CFG* Asset::cfg() {
  if (mCFG == nullptr)
    throw std::runtime_error("Tried to access CFG when nullptr");
  return mCFG;
}

Input::Input* Asset::input() {
  if (mInput == nullptr)
    throw std::runtime_error("Tried to access Input when nullptr");
  return mInput;
}

Lua::Lua* Asset::lua() {
  if (mLua == nullptr)
    throw std::runtime_error("Tried to access Lua when nullptr");
  return mLua;
}

ResourceManager* Asset::rManager() {
  if (mResourceManager == nullptr)
    throw std::runtime_error("Tried to access ResourceManager when nullptr");
  return mResourceManager;
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

void Asset::setResourceManager(ResourceManager* r) {
  mResourceManager = r;
}
