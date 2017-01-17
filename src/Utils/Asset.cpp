#include "Asset.hpp"
#include "Utils.hpp"

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

void Asset::setCFG(CFG* c) {
  mCFG = c;
}

void Asset::setInput(Input::Input* i) {
  mInput = i;
}
