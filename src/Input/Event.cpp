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
    , mType(Type::MouseScroll)
    , mCharacter("")
    , mPosition(mousePosition)
    , mScroll(scrollOffset)
    , mHandledState(State::NOCHANGE) {}

Input::Event::Event(const vec2& mousePosition)
    : mKeyCode(-1)
    , mMods(0)
    , mType(Type::MouseMovement)
    , mCharacter("")
    , mPosition(mousePosition)
    , mScroll(0, 0)
    , mHandledState(State::NOCHANGE) {}

Input::Event::Event(std::string s)
    : mKeyCode(-1)
    , mMods(0)
    , mType(Type::CharacterInput)
    , mPosition(-1, -1)
    , mScroll(0, 0)
    , mHandledState(State::NOCHANGE) {
  mCharacter = s;
}

Input::Event::~Event() {}

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

void Input::Event::sendStateChange(int stateChange) const {
  if (stateChange == State::NOCHANGE)
    return;

  mHandledState = stateChange;
}
