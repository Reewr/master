#pragma once

#include <string>

#include "../Input/Input.hpp"
#include "../Math/Math.hpp"

namespace Input {

  //! The event class is a class that is a wrapper around an input event.
  //! It differentiates between what type of input it is by which constructor
  //! is used and saves that as its type that can be retrieved using type()
  //! and compared against Input::Event::Type
  //!
  //! All the different events share the same data, however, some of the data
  //! only makes sense for a specific event. For instance, position() only makes
  //! sense for MouseMovement, MousePress, MouseRelease or MouseScroll.
  class Event {
  public:
    //! Describes the type of events that exists
    enum Type {
      Consumed = -1,
      MouseMovement,
      MousePress,
      MouseRelease,
      MouseScroll,
      KeyPress,
      KeyRelease,
      CharacterInput,
    };

    //! Calling this constructor tells the Event that it is of either
    //! a KeyPress or KeyRelease event
    Event(Input* i, int key, int action, int mods);

    //! Calling this constructor tells the Event that it is of either
    //! a MousePress or MouseRelease event
    Event(Input* i, const vec2& mousePosition, int key, int action, int mods);

    //! Calling this constructor tells the Event that it is of MouseScroll event
    Event(Input* i, const vec2& mousePosition, const vec2& scrollOffset);

    //! Calling the constructor without any additional information than the
    //! mouse
    //! position indicates that it is a MouseMovement event
    Event(Input* i, const vec2& mousePosition);

    //! Lastly, the CharacterInput event only consists of a single character
    Event(Input* i, std::string s);
    ~Event();

    bool operator==(int type) const;
    bool operator!=(int type) const;

    //! Returns the type of event
    int type() const;

    //! Checks whether the ALT modifier is pressed
    bool hasAlt() const;

    //! Checks whether the Control modifier is pressed
    bool hasCtrl() const;

    //! Checks whether the Super modifier is pressed
    bool hasSuper() const;

    //! Checks whether the Shift modifier is pressed
    bool hasShift() const;

    //! Returns the keycode for the event
    int key() const;

    //! Synonomous for key() but makes more sense in the
    //! case of mouse button
    int button() const;

    //! Checks if the event matches an Action defined by Input::Action.
    //! This is technically a wrapper around the Input class, since a
    //! lot of handlers that handles inputs will need to check actions.
    bool isAction(Action a) const;

    //! Checks if a key is continually held down
    bool isKeyHeldDown(int key) const;

    //! This is a simplification for checking for buttons. It firstly
    //! checks that it is a key press event and then
    //! compares the code to the key given.
    bool keyPressed(int glfwKey) const;

    //! This is a simplification for checking for buttons. It firstly
    //! checks that it is a mouse press event and then
    //! compares the code to the key given.
    bool buttonPressed(int glfwKey) const;

    //! Returns the mouse position, this only make sense to use if
    //! the event is a mouse event.
    const vec2& position() const;

    //! Checks whether the scroll was made left.
    //! This will always return false unless the event is a MouseScroll
    //! event
    bool scrollLeft() const;

    //! Checks whether the scroll was made right.
    //! This will always return false unless the event is a MouseScroll
    //! event
    bool scrollRight() const;

    //! Checks whether the scroll was made up.
    //! This will always return false unless the event is a MouseScroll
    //! event
    bool scrollUp() const;

    //! Checks whether the scroll was made down.
    //! This will always return false unless the event is a MouseScroll
    //! event
    bool scrollDown() const;

    // Checks if the event has been handled
    bool hasBeenHandled() const;

    // Returns the character if the type is Type::CharacterInput, otherwise
    // it returns an empty string
    std::string character() const;

    // If `stopPropagation` is called, it will change the type to equal
    // Event::Type::Consumed. This means that all == or type() calls
    // will be false when compared other Event::Type enums.
    //
    // In order to know what the previous state was, you can use this
    // function.
    //
    // Calling this prior to stopPropgation will throw an error
    int prevType() const;

    // Returns the stored handled state. By default this is NOCHANGE
    int state() const;

    //! Tells the Event that you want to change the state
    void sendStateChange(int stateChange) const;

    //! Calling this function will stop the propegation of events,
    //! This sets the state to Consumed.
    void stopPropgation() const;

  private:
    int mKeyCode;
    int mMods;

    std::string mCharacter;

    vec2 mPosition;
    vec2 mScroll;

    Input* mInput;

    mutable int mPrev;
    mutable int mType;
    mutable int mHandledState;
  };
}
