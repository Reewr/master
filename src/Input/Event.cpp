#include "Event.hpp"


#include "../OpenGLHeaders.hpp"

#include "../State/State.hpp"
#include "../Utils/Utils.hpp"
#include "Input.hpp"

Input::Event::Event(int key, int action, int mods)
    : mKeyCode(key)
    , mMods(mods)
    , mPosition(-1, -1)
    , mScroll(0, 0)
    , mHandledState(State::NOCHANGE) {
  mType      = action == GLFW_PRESS ? Type::KeyPress : Type::KeyRelease;
  mCharacter = glfwKeyToString(key);
}

Input::Event::Event(const vec2& mousePosition, int key, int action, int mods)
    : mKeyCode(key)
    , mMods(mods)
    , mPosition(mousePosition)
    , mScroll(0, 0)
    , mHandledState(State::NOCHANGE) {
  mType      = action == GLFW_PRESS ? Type::MousePress : Type::MouseRelease;
  mCharacter = glfwKeyToString(key);
}

Input::Event::Event(const vec2& mousePosition, const vec2& scrollOffset)
    : mKeyCode(-1)
    , mMods(0)
    , mCharacter("")
    , mPosition(mousePosition)
    , mScroll(scrollOffset)
    , mType(Type::MouseScroll)
    , mHandledState(State::NOCHANGE) {}

Input::Event::Event(const vec2& mousePosition)
    : mKeyCode(-1)
    , mMods(0)
    , mCharacter("")
    , mPosition(mousePosition)
    , mScroll(0, 0)
    , mType(Type::MouseMovement)
    , mHandledState(State::NOCHANGE) {}

Input::Event::Event(std::string s)
    : mKeyCode(-1)
    , mMods(0)
    , mPosition(-1, -1)
    , mScroll(0, 0)
    , mType(Type::CharacterInput)
    , mHandledState(State::NOCHANGE) {
  mCharacter = s;
}

Input::Event::~Event() {}

bool Input::Event::operator==(int type) const {
  return mType == type;
}

bool Input::Event::operator!=(int type) const {
  return mType != type;
}

int Input::Event::type() const {
  return mType;
}

bool Input::Event::hasAlt() const {
  return mMods & GLFW_MOD_ALT;
}
bool Input::Event::hasCtrl() const {
  return mMods & GLFW_MOD_CONTROL;
}
bool Input::Event::hasSuper() const {
  return mMods & GLFW_MOD_SUPER;
}
bool Input::Event::hasShift() const {
  return mMods & GLFW_MOD_SHIFT;
}

int Input::Event::key() const {
  return mKeyCode;
}

int Input::Event::button() const {
  return mKeyCode;
}

int Input::Event::state() const {
  return mHandledState;
}

bool Input::Event::keyPressed(int glfwKey) const {
  if (mType != Event::Type::KeyPress) {
    return false;
  }

  return glfwKey == key();
}

bool Input::Event::buttonPressed(int glfwKey) const {
  if (mType != Event::Type::MousePress) {
    return false;
  }

  return glfwKey == button();
}

const vec2& Input::Event::position() const {
  return mPosition;
}

bool Input::Event::scrollLeft() const {
  return mScroll.x == -1;
}

bool Input::Event::scrollRight() const {
  return mScroll.x == 1;
}

bool Input::Event::scrollUp() const {
  return mScroll.y == -1;
}

bool Input::Event::scrollDown() const {
  return mScroll.y == 1;
}

bool Input::Event::hasBeenHandled() const {
  return mType == Type::Consumed;
}

std::string Input::Event::character() const {
  return mCharacter;
}

int Input::Event::prevType() const {
  if (!hasBeenHandled())
    throw Error("Calling prevType on non-handled event is not legal");

  return mPrev;
}

void Input::Event::sendStateChange(int stateChange) const {
  if (stateChange == State::NOCHANGE)
    return;

  mHandledState = stateChange;
}

#include <execinfo.h>
#include <unistd.h>

// TESTING
// @TODO: REMOVE
void trace() {
  void*  array[5];
  size_t size;

  size = backtrace(array, 5);

  log("Handled propagation");
  backtrace_symbols_fd(array, size, STDERR_FILENO);
}
void Input::Event::stopPropgation() const {
  /* if (mType != Type::MouseMovement) */
  /*   trace(); */
  mPrev = mType;
  mType = Type::Consumed;
}
