#ifndef INPUT_HPP
#define INPUT_HPP

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "OpenGLHeaders.hpp"

#include "Math/Math.hpp"

struct Asset;
struct ActB;
struct GLFWwindow;
struct CFG;

/**
 * @brief
 *   Keys is a representation of an action together with two keys.
 *   Since every action can have two possible keybinding, this helps
 *   save that.
 */
struct Keys {
  Keys(int k1 = -1, int k2 = -1);
  Keys(const ActB& a);
  int key1, key2;
};

struct InputFunc {
  int                      isPressed;
  std::function<int(void)> func;
};

class Input {
public:
  Input(GLFWwindow* w, CFG* c);

  //! Reinitalizes the input to load keys again.
  void initialize();

  //! Returns both GLFW_KEY_*'s bound to an action
  Keys getKey(int action);

  //! Returns a action bound to a GLFW_KEY_*
  int getAction(int glfwKey);

  //! Sets the action to be bound to 2x GLFW_KEY_*
  void setKey(int action, int glfwKey1 = NOT_BOUND, int glfwKey2 = NOT_BOUND);

  //! Unbinds actions bound to a GLFW_KEY_*
  void unbindExisting(int glfwKey);

  //! Returns true if action is bound to GLFW_KEY_*
  bool checkKey(int desiredAction, int glfwKey);

  //! Returns true if any of the actions is bound to GLFW_KEY_*
  bool checkKeys(std::vector<int> desiredActions, int glfwKey);

  //! Returns all actions that are currently pressed down.
  std::vector<int> getPressedActions();

  //! Returns true if either of the two keys bounded to action is held down
  bool isActionPressed(int action);

  // Returns true if mouse button is held down.
  bool isMousePressed(int key);

  //! Returns true if key is held down.
  bool isKeyPressed(int key);

  //! Returns the string representation for an action
  std::vector<std::string> getActionsString();

  //! Returns the string representation of a key
  std::string glfwKeyToString(int glfwKey);

  //! Returns the string representation of two keys for an action
  std::vector<std::string> getActionKeysToString(int action);

  //! Returns last reported mouse coords
  vec2 getMouseCoords();

  //! sets the current mouse coords. usefull to call when a button is pressed.
  void setPressedCoord(int btn);

  //! Returns the latest recorded pressed coordinates for the button. (0, 0) if
  //! not recorded.
  vec2 getPressedCoord(int btn);

  //! Adds an action. Unlike `setKeys` this also allows you to set
  //! the string representation of that action
  void addAction(int action, Keys k, std::string text = "");

  //! Adds a callback function to a specific key
  void
  addKeyCB(int key, std::function<int(void)>&& f, int isPressed = GLFW_PRESS);

  //! Performs the callback that is assigned to a spesific key, if any.
  int doKeyCB(int key, int action);

  //! Clears all the previously set keybindings
  void resetKeyCB();

  //! Actions
  enum {
    NOT_BOUND = -1,
    PAUSEMENU,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    ROTATE,
    SCREENSHOT,
    CONSOLE
  };

  // Massive maps that contain a key to string and string to key
  // representation of every available glfw key.
  static std::map<int, std::string> keyStrings;
  static std::map<std::string, int> keyMap;

private:
  // used to store the functions received through addKeyCB()
  std::map<int, InputFunc> functionCalls;
  std::map<int, Keys>      keys;
  std::map<int, int>       glfwKeys;
  std::map<int, vec2>      pressedCoords;
  std::vector<std::string> actionsString;

  GLFWwindow* window;
  CFG*        cfg;
};
#endif