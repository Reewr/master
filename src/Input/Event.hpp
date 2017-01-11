#pragma once

#include <string>

#include "../Math/Math.hpp"

class Event {
public:
  //! This struct contains all the information about
  //! the key input that can be recieved from glfw
  struct Key {
    Key();
    int         code;
    int         scan;
    int         action;
    int         mods;
    std::string character;
  };

  //! This struct contains all the information about
  //! the mouse input that can be recieved from glfw
  struct Mouse {
    Mouse();
    vec2 position;
    vec2 scroll;
    int  button;
    int  action;
    int  mods;
  };

  Event();

  ~Event();

  //! Returns true if glfw has sent key input information
  bool hasKeyInput() const;

  //! Returns true if glfw has sent mouse input information
  bool hasMouseInput() const;

  //! Returns true if glfw has sent scroll input information
  bool hasScrollInput() const;

  // Retrieves the key input information. This should only be used
  // after you have verified that hasKeyInput() is true
  const Key& key() const;

  // Retrieves the mouse information. This should only be used
  // after you have verified that hasMouseInput() or hasScrollInput()
  // is true
  const Mouse& mouse() const;

  //! Tells the Event that you want to change the state
  void sendStateChange(int stateChange) const;

  // Returns the stored handled state. By default this is NOCHANGE
  int state() const;

  //! All functions below this point should only ever be used
  //! to initialize the Event.
  void addKeyInput(int key, int scan, int action, int mods);
  void addMouseInput(double x, double y);
  void addMouseButtonInput(int button, int action, int mods);
  void addScrollInput(double x, double y);
  void addCharacter(unsigned int x);

private:
  bool mRecievedKeyInput;
  bool mRecievedMouseInput;
  bool mRecievedScrollInput;

  Key   mKey;
  Mouse mMouse;

  mutable int mHandledState;
};
