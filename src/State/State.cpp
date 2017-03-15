#include "State.hpp"
State::State() : Logging::Log("RenameMeState") {}
State::~State() {}

Drawable3D* State::drawable(unsigned int index) {
  if (index >= mDrawable3D.size())
    return nullptr;
  return mDrawable3D[index];
}

unsigned int State::numDrawables() {
  return mDrawable3D.size();
}

GUI* State::gui(unsigned int index) {
  if (index >= mGUIElements.size())
    return nullptr;
  return mGUIElements[index];
}

unsigned int State::numGUIs() {
  return mGUIElements.size();
}
