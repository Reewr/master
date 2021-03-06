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
   * @param i
   *
   * @param key
   *   The integer representation of the key. This uses the GLFW_KEY_*
   *   integers to represent the keys.
   *
   * @param action
   *   The action, either GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
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
      , mHandledState(States::NoChange) {
    mType = action == GLFW_PRESS ?
              Type::KeyPress :
              action == GLFW_RELEASE ? Type::KeyRelease : Type::KeyRepeat;
    mCharacter = glfwKeyToString(key);

    if (mInput != nullptr && mType == Type::KeyPress)
      mInput->setPressedCoord(key);
  }

  /**
   * @brief
   *   The constructors of the Event class determines what type
   *   of event it is.
   *
   *   This constructor will make the Event class a Mouse event of
   *   either MousePress or MouseRelease
   *
   * @param i
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
  Event::Event(Input*           i,
               const mmm::vec2& mousePosition,
               int              key,
               int              action,
               int              mods)
      : mKeyCode(key)
      , mMods(mods)
      , mPosition(mousePosition)
      , mScroll(0, 0)
      , mInput(i)
      , mHandledState(States::NoChange) {
    mType      = action == GLFW_PRESS ? Type::MousePress : Type::MouseRelease;
    mCharacter = glfwKeyToString(key);

    if (mInput != nullptr && mType == Type::MousePress)
      mInput->setPressedCoord(key);
  }

  /**
   * @brief
   *   The constructors of the Event class determines what type
   *   of event it is.
   *
   *   This constructor will make the Event class a MouseScroll event
   *
   * @param i
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
  Event::Event(Input*           i,
               const mmm::vec2& mousePosition,
               const mmm::vec2& scrollOffset)
      : mKeyCode(-1)
      , mMods(0)
      , mCharacter("")
      , mPosition(mousePosition)
      , mScroll(scrollOffset)
      , mInput(i)
      , mType(Type::MouseScroll)
      , mHandledState(States::NoChange) {}

  /**
   * @brief
   *   The constructors of the Event class determines what type
   *   of event it is.
   *
   *   This constructor will make the Event class a MouseMovement event
   *
   * @param i
   * @param mousePosition
   *   Where the mouse moved to.
   */
  Event::Event(Input* i, const mmm::vec2& mousePosition)
      : mKeyCode(-1)
      , mMods(0)
      , mCharacter("")
      , mPosition(mousePosition)
      , mScroll(0, 0)
      , mInput(i)
      , mType(Type::MouseMovement)
      , mHandledState(States::NoChange) {}

  /**
   * @brief
   *   The constructors of the Event class determines what type
   *   of event it is.
   *
   *   This constructor will make the Event a CharacterInput
   *
   * @param i
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
      , mHandledState(States::NoChange) {
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
  bool Event::operator==(int type) const { return mType == type; }

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
  bool Event::operator!=(int type) const { return mType != type; }

  /**
   * @brief
   *   Checks whether the event matches an action. Uses the input manager
   *   to determine what keys is needed to trigger that action.
   *
   * @param a
   *
   * @return
   */
  bool Event::isAction(Action a) const {
    return mInput != nullptr && mInput->checkKey(a, mKeyCode);
  }

  /**
   * @brief
   *   Checks if the key is held down. This check with GLFW
   *   whether the key is held down or not.
   *
   * @param key
   *
   * @return
   */
  bool Event::isKeyHeldDown(int key) const {
    return mInput != nullptr && mInput->isKeyPressed(key);
  }

  /**
   * @brief
   *   Returns the type of the event
   *
   * @return
   */
  int Event::type() const { return mType; }

  /**
   * @brief
   *   Returns the type of event as a string
   *   which happens to be the name of the Enum
   *
   * @return
   */
  std::string Event::typeAsString() const {
    switch (mType) {
      case Type::Consumed:
        return "Consumed";
      case Type::CharacterInput:
        return "CharacterInput";
      case Type::KeyPress:
        return "KeyPress";
      case Type::KeyRelease:
        return "KeyRelease";
      case Type::KeyRepeat:
        return "KeyRepeat";
      case Type::MousePress:
        return "MousePress";
      case Type::MouseRelease:
        return "MouseRelease";
      case Type::MouseMovement:
        return "MouseMovement";
      case Type::MouseScroll:
        return "MouseScroll";
      default:
        return "MISSING";
    }
  }

  /**
   * @brief
   *   Returns whether or not the alt button was pressed
   *   at the time of the event
   *
   * @return
   */
  bool Event::hasAlt() const { return mMods & GLFW_MOD_ALT; }

  /**
   * @brief
   *   Returns whether or not the control button was pressed
   *   at the time of the event
   *
   * @return
   */
  bool Event::hasCtrl() const { return mMods & GLFW_MOD_CONTROL; }

  /**
   * @brief
   *   Returns whether or not the super button was pressed
   *   at the time of the event
   *
   * @return
   */
  bool Event::hasSuper() const { return mMods & GLFW_MOD_SUPER; }

  /**
   * @brief
   *   Returns whether or not the shift button was pressed
   *   at the time of the event
   *
   * @return
   */
  bool Event::hasShift() const { return mMods & GLFW_MOD_SHIFT; }

  /**
   * @brief
   *   Returns the keycode of the key input. This may be
   *   -1 if the event does not have a keycode, for instance in
   *   the case of MouseMovement or MouseScroll
   *
   * @return
   */
  int Event::key() const { return mKeyCode; }

  /**
   * @brief
   *   This function is synonomous with key() and does exactly
   *   the same, but it makes more sense to use this when you
   *   expect a MousePress / MouseRelease
   *
   * @return
   */
  int Event::button() const { return mKeyCode; }

  /**
   * @brief
   *   Returns the state that is stored. This value represents whether
   *   or not a stateChange will happen when the event is over.
   *
   *   This defaults to `States::NoChange` and can be changed through
   *   sendStateChange()
   *
   * @return
   */
  int Event::state() const { return mHandledState; }

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
   *   Checks if the event is of a type that is a Key event, meaning either
   *   press, repeat or release.
   *
   * @return
   */
  bool Event::isKeyEvent() const {
    return mType == Type::KeyPress || mType == Type::KeyRepeat ||
           mType == Type::KeyRelease;
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
   *   The mmm::vec2 will default to 0 when it is not a mouse event.
   *
   * @return
   */
  const mmm::vec2& Event::position() const { return mPosition; }


  /**
   * @brief
   *   Returns the position of the mouse retrieved from GLFW. This function
   *   can be used with any event and it will retrieve the coordinates
   *   by quering GLFW for it.
   *
   *   Please note that if this is a mouse event, you are better of using
   *   `position()`.
   *
   * @return
   */
  mmm::vec2 Event::currentMousePosition() const {
    if (mInput == nullptr)
      throw std::runtime_error("Invalid input manager");

    return mInput->getMouseCoords();
  }

  /**
   * @brief
   *   Returns the position of the mouse when the given button was
   *   pressed down. This function is useful when you are trying
   *   to retrieve the difference between events where one was the press
   *   and the other was release.
   *
   * @param glfwKey
   *
   * @return
   */
  const mmm::vec2& Event::getPressedPosition(int glfwKey) const {
    if (mInput == nullptr)
      throw std::runtime_error("Invalid input manager");

    return mInput->getPressedCoord(glfwKey);
  }

  /**
   * @brief
   *   Checks whether a scroll left event happened
   *
   * @return
   */
  bool Event::scrollLeft() const { return mScroll.x == -1; }

  /**
   * @brief
   *   Checks whether a scroll right event happened
   *
   * @return
   */
  bool Event::scrollRight() const { return mScroll.x == 1; }

  /**
   * @brief
   *   Checks whether a scroll up event happened
   *
   * @return
   */
  bool Event::scrollUp() const { return mScroll.y == -1; }

  /**
   * @brief
   *   Checks whether a scroll down event happened
   *
   * @return
   */
  bool Event::scrollDown() const { return mScroll.y == 1; }

  /**
   * @brief
   *   The event has the stopPropgation() function. When
   *   this is called, this function will return true, otherwise
   *   it will return false.
   *
   * @return
   */
  bool Event::hasBeenHandled() const { return mType == Type::Consumed; }

  /**
   * @brief
   *   Gets the character representation of the key/button or the character
   *   input. This is empty string if a representation cannot be made.
   *
   * @return
   */
  std::string Event::character() const { return mCharacter; }

  /**
   * @brief
   *   Returns the keys for a specific action.
   *
   * @param a
   *
   * @return
   */
  const Keys& Event::getKeysForAction(Action a) const {
    if (mInput == nullptr)
      throw std::runtime_error("Invalid input manager");

    return mInput->getKey(a);
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
      throw std::runtime_error("Calling prevType on unhandled event");

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
    mHandledState = stateChange;
  }

  /**
   * @brief
   *   This function tells the event that it has been handled.
   *   Once an event is handled, it will no longer match any type
   *   of Event::Type.
   */
  void Event::stopPropgation() const {
    mPrev = mType;
    mType = Type::Consumed;
  }
}
