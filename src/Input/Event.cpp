#include "Event.hpp"

#include "../State/State.hpp"
#include "../Utils/Utils.hpp"

Event::Key::Key() : code(-1), scan(-1), action(-1), mods(0), character("") {}

Event::Mouse::Mouse() {}

Event::Event() {
  mHandledState = State::NOCHANGE;
}

Event::~Event() {}

void Event::addKeyInput(int key, int scan, int action, int mods) {
  mKey.mods         = mods;
  mKey.code         = key;
  mKey.scan         = scan;
  mKey.action       = action;
  mRecievedKeyInput = true;
}

void Event::addMouseInput(double x, double y) {
  mMouse.position     = vec2(x, y);
  mRecievedMouseInput = true;
}

void Event::addScrollInput(double x, double y) {
  mMouse.scroll        = vec2(x, y);
  mRecievedScrollInput = true;
}

void Event::addCharacter(unsigned int x) {
  mKey.character = Utils::utf8toStr(x);
}

bool Event::hasKeyInput() const {
  return mRecievedKeyInput;
}

bool Event::hasMouseInput() const {
  return mRecievedMouseInput;
}

bool Event::hasScrollInput() const {
  return mRecievedScrollInput;
}

int Event::state() const {
  return mHandledState;
}

const Event::Key& Event::key() const {
  return mKey;
}
const Event::Mouse& Event::mouse() const {
  return mMouse;
}

void Event::sendStateChange(int stateChange) const {
  if (stateChange == State::NOCHANGE)
    return;

  mHandledState = stateChange;
}
