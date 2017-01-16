#include "Event.hpp"


#include "../OpenGLHeaders.hpp"

#include "../State/State.hpp"
#include "../Utils/Utils.hpp"
#include "Input.hpp"

namespace Input {
/**
 * @brief
 *   The constructors of the Event class determines what type
 *   of event it is.
 *
 *   This constructor will make the Event class a Key event of
 *   either KeyPress or KeyRelease.
 *
 * @param key
 *   The integer representation of the key. This uses the GLFW_KEY_*
 *   integers to represent the keys.
 *
 * @param action
 *   The action, either GLFW_PRESS or GLFW_RELEASE
 *
 * @param mods
 *   The mods active at the time of the event indicated
 *   by GLFW_MODIFIER_ALT etc.
 */
Event::Event(Input* i, int key, int action, int mods)
    : mKeyCode(key)
    , mMods(mods)
    , mPosition(-1, -1)
    , mScroll(0, 0)
    , mInput(i)
    , mHandledState(State::NOCHANGE) {
  mType      = action == GLFW_PRESS ? Type::KeyPress : Type::KeyRelease;
  mCharacter = glfwKeyToString(key);
}

/**
 * @brief
 *   The constructors of the Event class determines what type
 *   of event it is.
 *
 *   This constructor will make the Event class a Mouse event of
 *   either MousePress or MouseRelease
 *
 * @param mousePosition
 *   The position of the mouse at the time of the click
 *
 * @param key
 *   Which key, or button that was pressed on the mouse indicated
 *   by GLFW_MOUSE_*
 *
 * @param action
 *   The action, either GLFW_PRESS or GLFW_RELEASE
 *
 * @param mods
 *   The mods active at the time of the event indicated
 *   by GLFW_MODIFIER_ALT etc.
 */
Event::Event(Input* i, const vec2& mousePosition, int key, int action, int mods)
    : mKeyCode(key)
    , mMods(mods)
    , mPosition(mousePosition)
    , mScroll(0, 0)
    , mInput(i)
    , mHandledState(State::NOCHANGE) {
  mType      = action == GLFW_PRESS ? Type::MousePress : Type::MouseRelease;
  mCharacter = glfwKeyToString(key);
}

/**
 * @brief
 *   The constructors of the Event class determines what type
 *   of event it is.
 *
 *   This constructor will make the Event class a MouseScroll event
 *
 * @param mousePosition
 *   Where the mouse was located when the scroll happened
 *
 * @param scrollOffset
 *   What type of scroll was performed. X = Horizontal, Y = Vertical.
 *   X = -1, Left, X = 1, Right.
 *   Y = -1, Down, Y = 1, Up
 *
 *   All other values for X and Y are ignored.
 */
Event::Event(Input* i, const vec2& mousePosition, const vec2& scrollOffset)
    : mKeyCode(-1)
    , mMods(0)
    , mCharacter("")
    , mPosition(mousePosition)
    , mScroll(scrollOffset)
    , mInput(i)
    , mType(Type::MouseScroll)
    , mHandledState(State::NOCHANGE) {}

/**
 * @brief
 *   The constructors of the Event class determines what type
 *   of event it is.
 *
 *   This constructor will make the Event class a MouseMovement event
 *
 * @param mousePosition
 *   Where the mouse moved to.
 */
Event::Event(Input* i, const vec2& mousePosition)
    : mKeyCode(-1)
    , mMods(0)
    , mCharacter("")
    , mPosition(mousePosition)
    , mScroll(0, 0)
    , mInput(i)
    , mType(Type::MouseMovement)
    , mHandledState(State::NOCHANGE) {}

/**
 * @brief
 *   The constructors of the Event class determines what type
 *   of event it is.
 *
 *   This constructor will make the Event a CharacterInput
 *
 * @param s
 *   The character inputted
 */
Event::Event(Input* i, std::string s)
    : mKeyCode(-1)
    , mMods(0)
    , mPosition(-1, -1)
    , mScroll(0, 0)
    , mInput(i)
    , mType(Type::CharacterInput)
    , mHandledState(State::NOCHANGE) {
  mCharacter = s;
}

Event::~Event() {}

/**
 * @brief
 *   To make it easier to compare types, the == operator has
 *   been overloaded on integers and will compare to the stored
 *   type.
 *
 *   This is equal to doing `event.type() == Event::Type::SomeType`
 *
 * @param type
 *   The type to compare to
 *
 * @return
 *   True if equal, else false
 */
bool Event::operator==(int type) const {
  return mType == type;
}

/**
 * @brief
 *   To make it easier to compare types, the != operator has
 *   been overloaded on integers and will compare to the stored
 *   type.
 *
 *   This is equal to doing `event.type() != Event::Type::SomeType`
 *
 * @param type
 *   The type to compare to
 *
 * @return
 *   false if equal, else true
 */
bool Event::operator!=(int type) const {
  return mType != type;
}

/**
 * @brief
 *   Returns the type of the event
 *
 * @return
 */
int Event::type() const {
  return mType;
}

/**
 * @brief
 *   Returns whether or not the alt button was pressed
 *   at the time of the event
 *
 * @return
 */
bool Event::hasAlt() const {
  return mMods & GLFW_MOD_ALT;
}

/**
 * @brief
 *   Returns whether or not the control button was pressed
 *   at the time of the event
 *
 * @return
 */
bool Event::hasCtrl() const {
  return mMods & GLFW_MOD_CONTROL;
}

/**
 * @brief
 *   Returns whether or not the super button was pressed
 *   at the time of the event
 *
 * @return
 */
bool Event::hasSuper() const {
  return mMods & GLFW_MOD_SUPER;
}

/**
 * @brief
 *   Returns whether or not the shift button was pressed
 *   at the time of the event
 *
 * @return
 */
bool Event::hasShift() const {
  return mMods & GLFW_MOD_SHIFT;
}

/**
 * @brief
 *   Returns the keycode of the key input. This may be
 *   -1 if the event does not have a keycode, for instance in
 *   the case of MouseMovement or MouseScroll
 *
 * @return
 */
int Event::key() const {
  return mKeyCode;
}

/**
 * @brief
 *   This function is synonomous with key() and does exactly
 *   the same, but it makes more sense to use this when you
 *   expect a MousePress / MouseRelease
 *
 * @return
 */
int Event::button() const {
  return mKeyCode;
}

/**
 * @brief
 *   Returns the state that is stored. This value represents whether
 *   or not a stateChange will happen when the event is over.
 *
 *   This defaults to `State::NOCHANGE` and can be changed through
 *   sendStateChange()
 *
 * @return
 */
int Event::state() const {
  return mHandledState;
}

/**
 * @brief
 *   Checks if a key is pressed by sending in the GLFW version of it.
 *   Usage example:
 *
 *   event.keyPressed(GLFW_KEY_ENTER);
 *
 *   Returns false if the event is not of a KeyPress or if the key
 *   does not match the current pressed key.
 *
 * @param glfwKey
 *
 * @return
 */
bool Event::keyPressed(int glfwKey) const {
  if (mType != Event::Type::KeyPress) {
    return false;
  }

  return glfwKey == key();
}

/**
 * @brief
 *   Checks if a button is pressed by sending in the GLFW version of it.
 *   Usage example:
 *
 *   event.keyPressed(GLFW_MOUSE_BUTTON_LEFT);
 *
 *   Returns false if the event is not of a MousePress or if the button
 *   does not match the current pressed button.
 *
 * @param glfwKey
 *
 * @return
 */
bool Event::buttonPressed(int glfwKey) const {
  if (mType != Event::Type::MousePress) {
    return false;
  }

  return glfwKey == button();
}

/**
 * @brief
 *   Returns the position of the mouse at the time of the event.
 *   This function only makes sense to use when you know its a mouse
 *   type event.
 *
 *   The vec2 will default to 0 when it is not a mouse event.
 *
 * @return
 */
const vec2& Event::position() const {
  return mPosition;
}

/**
 * @brief
 *   Checks whether a scroll left event happened
 *
 * @return
 */
bool Event::scrollLeft() const {
  return mScroll.x == -1;
}

/**
 * @brief
 *   Checks whether a scroll right event happened
 *
 * @return
 */
bool Event::scrollRight() const {
  return mScroll.x == 1;
}

/**
 * @brief
 *   Checks whether a scroll up event happened
 *
 * @return
 */
bool Event::scrollUp() const {
  return mScroll.y == -1;
}

/**
 * @brief
 *   Checks whether a scroll down event happened
 *
 * @return
 */
bool Event::scrollDown() const {
  return mScroll.y == 1;
}

/**
 * @brief
 *   The event has the stopPropgation() function. When
 *   this is called, this function will return true, otherwise
 *   it will return false.
 *
 * @return
 */
bool Event::hasBeenHandled() const {
  return mType == Type::Consumed;
}

/**
 * @brief
 *   Gets the character representation of the key/button or the character
 *   input. This is empty string if a representation cannot be made.
 *
 * @return
 */
std::string Event::character() const {
  return mCharacter;
}

/**
 * @brief
 *   Since the stopPropgation() function changes the type
 *   so that all comparisons return false, this checks
 *   the type before it was handled.
 *
 *   This will throw an error if the event has not yet
 *   been handled. Therefore you should call hasBeenHandled()
 *   to make sure that the event has really been handled.
 *
 * @return
 */
int Event::prevType() const {
  if (!hasBeenHandled())
    throw Error("Calling prevType on non-handled event is not legal");

  return mPrev;
}

/**
 * @brief
 *   Tells the event that it should tell the Engine that it is
 *   time to do a state change. What this will mean is that when
 *   the event is over, the engine will perform the requested
 *   state change, which may be to QUIT, QUITALL or do other stuff.
 *
 * @param stateChange
 */
void Event::sendStateChange(int stateChange) const {
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
/**
 * @brief
 *   This function tells the event that it has been handled.
 *   Once an event is handled, it will no longer match any type
 *   of Event::Type.
 */
void Event::stopPropgation() const {
  /* if (mType != Type::MouseMovement) */
  /*   trace(); */
  mPrev = mType;
  mType = Type::Consumed;
}

}
