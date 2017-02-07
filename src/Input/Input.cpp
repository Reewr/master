#include "Input.hpp"

#include "OpenGLHeaders.hpp"

#include "Utils/Asset.hpp"
#include "Utils/CFG.hpp"

using mmm::vec2;

namespace Input {

  Keys::Keys(int k1, int k2) {
    key1 = k1;
    key2 = k2;
  }

  Keys::Keys(const ActB& a) {
    key1 = a.key1;
    key2 = a.key2;
  }

  Input::Input(GLFWwindow* w, CFG* c) {
    window = w;
    cfg    = c;
    initialize();
  }

  /**
   * @brief
   *   Returns a Keys structure for the given action. If the action
   *   cannot be found, a Keys structure of two KEY_UNKNOWN variables
   *   are returned.
   *
   * @param action
   *   The action to check for
   *
   * @return
   */
  const Keys& Input::getKey(int action) {
    if (keys.count(action))
      return keys[action];
    return keys[Action::NotBound];
  }

  /**
   * @brief
   *   Sets zero, one or two keys for a specific action. Both key variables
   *   are optional. Sending none of the key values in will effectively
   *   unbind the action, meaning that the action can never be invoked.
   *
   *   Sending one or more keys will unbind the keys from other existing
   *   actions and rebind them to this action.
   *
   *   Note: A side effect of this function is that you can effectively
   *         unbind other actions than the one you are setting if you are
   *         using one or two of the only keys that the other action depends
   *         on.
   *
   * @param action
   *   The action to bind and/or unbind
   *
   * @param glfwKey1
   *   A glfw key, is optional
   *
   * @param glfwKey2
   *   A glfw key, is optional
   */
  void Input::setKey(int action, int glfwKey1, int glfwKey2) {
    unbindExisting(glfwKey1);
    unbindExisting(glfwKey2);

    keys[action] = { glfwKey1, glfwKey2 };

    glfwKeys[glfwKey1] = action;
    glfwKeys[glfwKey2] = action;
  }

  /**
   * @brief
   *   Checks if a glfw key is bound to an action. If so, it will
   *   unbind it from the said action.
   *
   * @param glfwKey
   */
  void Input::unbindExisting(int glfwKey) {
    if (glfwKeys.count(glfwKey)) {
      if (keys[glfwKeys[glfwKey]].key1 == glfwKey)
        keys[glfwKeys[glfwKey]].key1 = Action::NotBound;
      else if (keys[glfwKeys[glfwKey]].key2 == glfwKey)
        keys[glfwKeys[glfwKey]].key2 = Action::NotBound;
    }
    glfwKeys[glfwKey] = Action::NotBound;
  }

  /**
   * @brief
   *   Checks whether a glfw key is a mapping to a specific action.
   *   This can be used when you want to check if some input X represents
   *   the action you need.
   *
   * @param desiredAction
   *   An action found in Input
   *
   * @param glfwKey
   *
   * @return
   */
  bool Input::checkKey(int desiredAction, int glfwKey) {
    return (desiredAction == getAction(glfwKey));
  }

  /**
   * @brief
   *   Retrieves the action for a specific key.
   *   May return Actions::NotBound which means there are no actions
   *   bound to the given key.
   *
   * @param glfwKey
   *
   * @return
   */
  int Input::getAction(int glfwKey) {
    if (glfwKey == GLFW_KEY_UNKNOWN)
      return Action::NotBound;

    if (glfwKeys.count(glfwKey))
      return glfwKeys[glfwKey];

    return Action::NotBound;
  }

  /**
   * @brief
   *   This works much like checkKey, but instead of only checking one
   *   action it checks multiple. Returning true if any of the actions
   *   is bound to the given key.
   *
   * @param desiredActions
   *   A list of actions found in Input
   *
   * @param glfwKey
   *
   * @return
   */
  bool Input::checkKeys(std::vector<int> desiredActions, int glfwKey) {
    for (int i : desiredActions)
      if (checkKey(i, glfwKey))
        return true;
    return false;
  }

  /**
   * @brief
   *   Goes through all the available keys and checks which actions
   *   are pressed, returning a vector of the actions.
   *
   * @return
   */
  std::vector<int> Input::getPressedActions() {
    std::vector<int> actions;
    for (auto& k : keys)
      if (isActionPressed(k.first))
        actions.push_back(k.first);
    return actions;
  }

  /**
   * @brief
   *   Checks whether an action is pressed.
   *
   * @param action
   *
   * @return
   */
  bool Input::isActionPressed(int action) {
    Keys k = getKey(action);
    if (k.key1 < 8 && (isMousePressed(k.key1) || isMousePressed(k.key2)))
      return true;
    if (k.key1 > 10 && (isKeyPressed(k.key1) || isKeyPressed(k.key2)))
      return true;
    return false;
  }

  /**
   * @brief
   *   Checks whether the mouse key is pressed down
   *
   * @param key
   *
   * @return
   */
  bool Input::isMousePressed(int key) {
    if (key == -1)
      return false;
    if (glfwGetMouseButton(window, key) == GLFW_PRESS)
      return true;
    return false;
  }

  /**
   * @brief
   *   Checks whether a key is pressed down.
   *
   * @param key
   *
   * @return
   */
  bool Input::isKeyPressed(int key) {
    if (key == -1)
      return false;
    if (glfwGetKey(window, key) == GLFW_PRESS)
      return true;
    return false;
  }

  /**
   * @brief
   *   Returns the last recorded mouse positions
   *
   * @return
   */
  vec2 Input::getMouseCoords() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return vec2(x, y);
  }

  /**
   * @brief
   *   This can be used to store coordinate. This can
   *   be overwritten by other calls to this function
   *   with the same button.
   *
   * @param btn
   */
  void Input::setPressedCoord(int btn) {
    pressedCoords[btn] = getMouseCoords();
  }

  /**
   * @brief
   *   Returns the pressed coordinates for a mouse button
   *   that was recorded by setPressedCoord()
   *
   * @param btn
   *
   * @return
   */
  const vec2& Input::getPressedCoord(int btn) {
    // return an object to a non-existant button
    if (!pressedCoords.count(btn))
      return pressedCoords.at(-1);

    return pressedCoords.at(btn);
  }

  /**
   * @brief
   *   Get the names of all the actions
   *
   * @return
   */
  std::vector<std::string> Input::getActionsString() { return actionsString; }

  /**
   * @brief
   *   Like glfwKeyToString(), this function returns the string
   *   representation of a button. However, this returns the representation
   *   for two keys on a specific action.
   *
   * @param action
   *
   * @return
   */
  std::vector<std::string> Input::getActionKeysToString(int action) {
    std::vector<std::string> v;
    v.push_back(glfwKeyToString(getKey(action).key1));
    v.push_back(glfwKeyToString(getKey(action).key2));
    return v;
  }

  /**
   * @brief
   *   This adds keys for an action. unlike `setKey`, this also
   *   takes in the string representation of that action.
   *
   * @param action
   * @param k
   * @param text
   */
  void Input::addAction(int action, Keys k, std::string text) {
    setKey(action, k.key1, k.key2);
    actionsString.push_back(text);
  }

  /**
   * @brief
   *   Initializes the entire key set.
   *   @TODO
   *     this should possibly be moved to the states,
   *     or the engine if the given options are global
   */
  void Input::initialize() {
    CFG::Bindings* b = &cfg->bindings;

    actionsString.clear();
    pressedCoords[-1] = vec2();
    addAction(Action::NotBound, ActB(-1, -1), "Not bound");
    addAction(Action::PauseMenu, b->pauseMenu, "Pause Menu");
    addAction(Action::MoveUp, b->moveUp, "Move up");
    addAction(Action::MoveDown, b->moveDown, "Move down");
    addAction(Action::MoveLeft, b->moveLeft, "Move left");
    addAction(Action::MoveRight, b->moveRight, "Move right");
    addAction(Action::Rotate, { GLFW_MOUSE_BUTTON_3 }, "Rotate");
    addAction(Action::Screenshot, b->screenshot, "Take screenshot");
    addAction(Action::Console, b->showConsole, "Show console");
  }

  /**
   * @brief
   *   Turns a glfwkey to a string representation of itself.
   *   This is not always that accurate due to the different
   *   languages a keyboard can have. This assumes US standard.
   *
   * @param glfwKey
   *
   * @return
   */
  std::string glfwKeyToString(int glfwKey) {
    if (keyStrings.count(glfwKey))
      return keyStrings[glfwKey];

    return keyStrings[GLFW_KEY_UNKNOWN];
  }

  std::map<int, std::string> keyStrings =
    { { GLFW_KEY_UNKNOWN, "Unbound" },
      { GLFW_MOUSE_BUTTON_1, "MouseButton1" },
      { GLFW_MOUSE_BUTTON_2, "MouseButton2" },
      { GLFW_MOUSE_BUTTON_3, "MouseButton3" },
      { GLFW_MOUSE_BUTTON_4, "MouseButton4" },
      { GLFW_MOUSE_BUTTON_5, "MouseButton5" },
      { GLFW_MOUSE_BUTTON_6, "MouseButton6" },
      { GLFW_MOUSE_BUTTON_7, "MouseButton7" },
      { GLFW_MOUSE_BUTTON_8, "MouseButton8" },
      { GLFW_KEY_SPACE, "Space" },
      { GLFW_KEY_APOSTROPHE, "Apostrophe" },
      { GLFW_KEY_COMMA, "Comma" },
      { GLFW_KEY_MINUS, "Minus" },
      { GLFW_KEY_PERIOD, "Period" },
      { GLFW_KEY_SLASH, "slash" },
      { GLFW_KEY_0, "0" },
      { GLFW_KEY_1, "1" },
      { GLFW_KEY_2, "2" },
      { GLFW_KEY_3, "3" },
      { GLFW_KEY_4, "4" },
      { GLFW_KEY_5, "5" },
      { GLFW_KEY_6, "6" },
      { GLFW_KEY_7, "7" },
      { GLFW_KEY_8, "8" },
      { GLFW_KEY_9, "9" },
      { GLFW_KEY_SEMICOLON, "Semicolon" },
      { GLFW_KEY_EQUAL, "Equal" },
      { GLFW_KEY_A, "a" },
      { GLFW_KEY_B, "b" },
      { GLFW_KEY_C, "c" },
      { GLFW_KEY_D, "d" },
      { GLFW_KEY_E, "e" },
      { GLFW_KEY_F, "f" },
      { GLFW_KEY_G, "g" },
      { GLFW_KEY_H, "h" },
      { GLFW_KEY_I, "i" },
      { GLFW_KEY_J, "j" },
      { GLFW_KEY_K, "k" },
      { GLFW_KEY_L, "l" },
      { GLFW_KEY_M, "m" },
      { GLFW_KEY_N, "n" },
      { GLFW_KEY_O, "o" },
      { GLFW_KEY_P, "p" },
      { GLFW_KEY_Q, "q" },
      { GLFW_KEY_R, "r" },
      { GLFW_KEY_S, "s" },
      { GLFW_KEY_T, "t" },
      { GLFW_KEY_U, "u" },
      { GLFW_KEY_V, "v" },
      { GLFW_KEY_W, "w" },
      { GLFW_KEY_X, "x" },
      { GLFW_KEY_Y, "y" },
      { GLFW_KEY_Z, "z" },
      { GLFW_KEY_LEFT_BRACKET, "LeftBracket" },
      { GLFW_KEY_BACKSLASH, "|" },
      { GLFW_KEY_RIGHT_BRACKET, "RightBracket" },
      { GLFW_KEY_GRAVE_ACCENT, "GraveAccent" },
      { GLFW_KEY_ESCAPE, "Escape" },
      { GLFW_KEY_ENTER, "Enter" },
      { GLFW_KEY_TAB, "Tab" },
      { GLFW_KEY_BACKSPACE, "Backspace" },
      { GLFW_KEY_INSERT, "Insert" },
      { GLFW_KEY_DELETE, "Delete" },
      { GLFW_KEY_RIGHT, "Right" },
      { GLFW_KEY_LEFT, "Left" },
      { GLFW_KEY_DOWN, "Down" },
      { GLFW_KEY_UP, "Up" },
      { GLFW_KEY_PAGE_UP, "PageUp" },
      { GLFW_KEY_PAGE_DOWN, "PageDown" },
      { GLFW_KEY_HOME, "Home" },
      { GLFW_KEY_END, "End" },
      { GLFW_KEY_CAPS_LOCK, "CapsLock" },
      { GLFW_KEY_SCROLL_LOCK, "ScrollLock" },
      { GLFW_KEY_NUM_LOCK, "NumLock" },
      { GLFW_KEY_PRINT_SCREEN, "PrintScreen" },
      { GLFW_KEY_PAUSE, "Pause" },
      { GLFW_KEY_F1, "F1" },
      { GLFW_KEY_F2, "F2" },
      { GLFW_KEY_F3, "F3" },
      { GLFW_KEY_F4, "F4" },
      { GLFW_KEY_F5, "F5" },
      { GLFW_KEY_F6, "F6" },
      { GLFW_KEY_F7, "F7" },
      { GLFW_KEY_F8, "F8" },
      { GLFW_KEY_F9, "F9" },
      { GLFW_KEY_F10, "F10" },
      { GLFW_KEY_F11, "F11" },
      { GLFW_KEY_F12, "F12" },
      { GLFW_KEY_F13, "F13" },
      { GLFW_KEY_F14, "F14" },
      { GLFW_KEY_F15, "F15" },
      { GLFW_KEY_F16, "F16" },
      { GLFW_KEY_F17, "F17" },
      { GLFW_KEY_F18, "F18" },
      { GLFW_KEY_F19, "F19" },
      { GLFW_KEY_F20, "F20" },
      { GLFW_KEY_F21, "F21" },
      { GLFW_KEY_F22, "F22" },
      { GLFW_KEY_F23, "F23" },
      { GLFW_KEY_F24, "F24" },
      { GLFW_KEY_F25, "F25" },
      { GLFW_KEY_KP_0, "Kp0" },
      { GLFW_KEY_KP_1, "Kp1" },
      { GLFW_KEY_KP_2, "Kp2" },
      { GLFW_KEY_KP_3, "Kp3" },
      { GLFW_KEY_KP_4, "Kp4" },
      { GLFW_KEY_KP_5, "Kp5" },
      { GLFW_KEY_KP_6, "Kp6" },
      { GLFW_KEY_KP_7, "Kp7" },
      { GLFW_KEY_KP_8, "Kp8" },
      { GLFW_KEY_KP_9, "Kp9" },
      { GLFW_KEY_KP_DECIMAL, "KpDecimal" },
      { GLFW_KEY_KP_DIVIDE, "KpDivide" },
      { GLFW_KEY_KP_MULTIPLY, "KpMultiply" },
      { GLFW_KEY_KP_SUBTRACT, "KpSubtract" },
      { GLFW_KEY_KP_ADD, "KpAdd" },
      { GLFW_KEY_KP_ENTER, "KpEnter" },
      { GLFW_KEY_KP_EQUAL, "KpEqual" },
      { GLFW_KEY_LEFT_SHIFT, "LeftShift" },
      { GLFW_KEY_LEFT_CONTROL, "LeftControl" },
      { GLFW_KEY_LEFT_ALT, "LeftAlt" },
      { GLFW_KEY_LEFT_SUPER, "LeftSuper" },
      { GLFW_KEY_RIGHT_SHIFT, "RightShift" },
      { GLFW_KEY_RIGHT_CONTROL, "RightControl" },
      { GLFW_KEY_RIGHT_ALT, "RightAlt" },
      { GLFW_KEY_RIGHT_SUPER, "RightSuper" },
      { GLFW_KEY_MENU, "Menu" } };

  std::map<std::string, int> keyMap =
    { { "Unbound", GLFW_KEY_UNKNOWN },
      { "MouseButton1", GLFW_MOUSE_BUTTON_1 },
      { "MouseButton2", GLFW_MOUSE_BUTTON_2 },
      { "MouseButton3", GLFW_MOUSE_BUTTON_3 },
      { "MouseButton4", GLFW_MOUSE_BUTTON_4 },
      { "MouseButton5", GLFW_MOUSE_BUTTON_5 },
      { "MouseButton6", GLFW_MOUSE_BUTTON_6 },
      { "MouseButton7", GLFW_MOUSE_BUTTON_7 },
      { "MouseButton8", GLFW_MOUSE_BUTTON_8 },
      { "Space", GLFW_KEY_SPACE },
      { "Apostrophe", GLFW_KEY_APOSTROPHE },
      { "Comma", GLFW_KEY_COMMA },
      { "Minus", GLFW_KEY_MINUS },
      { "Period", GLFW_KEY_PERIOD },
      { "slash", GLFW_KEY_SLASH },
      { "0", GLFW_KEY_0 },
      { "1", GLFW_KEY_1 },
      { "2", GLFW_KEY_2 },
      { "3", GLFW_KEY_3 },
      { "4", GLFW_KEY_4 },
      { "5", GLFW_KEY_5 },
      { "6", GLFW_KEY_6 },
      { "7", GLFW_KEY_7 },
      { "8", GLFW_KEY_8 },
      { "9", GLFW_KEY_9 },
      { "Semicolon", GLFW_KEY_SEMICOLON },
      { "Equal", GLFW_KEY_EQUAL },
      { "a", GLFW_KEY_A },
      { "b", GLFW_KEY_B },
      { "c", GLFW_KEY_C },
      { "d", GLFW_KEY_D },
      { "e", GLFW_KEY_E },
      { "f", GLFW_KEY_F },
      { "g", GLFW_KEY_G },
      { "h", GLFW_KEY_H },
      { "i", GLFW_KEY_I },
      { "j", GLFW_KEY_J },
      { "k", GLFW_KEY_K },
      { "l", GLFW_KEY_L },
      { "m", GLFW_KEY_M },
      { "n", GLFW_KEY_N },
      { "o", GLFW_KEY_O },
      { "p", GLFW_KEY_P },
      { "q", GLFW_KEY_Q },
      { "r", GLFW_KEY_R },
      { "s", GLFW_KEY_S },
      { "t", GLFW_KEY_T },
      { "u", GLFW_KEY_U },
      { "v", GLFW_KEY_V },
      { "w", GLFW_KEY_W },
      { "x", GLFW_KEY_X },
      { "y", GLFW_KEY_Y },
      { "z", GLFW_KEY_Z },
      { "LeftBracket", GLFW_KEY_LEFT_BRACKET },
      { "|", GLFW_KEY_BACKSLASH },
      { "RightBracket", GLFW_KEY_RIGHT_BRACKET },
      { "GraveAccent", GLFW_KEY_GRAVE_ACCENT },
      { "Escape", GLFW_KEY_ESCAPE },
      { "Enter", GLFW_KEY_ENTER },
      { "Tab", GLFW_KEY_TAB },
      { "Backspace", GLFW_KEY_BACKSPACE },
      { "Insert", GLFW_KEY_INSERT },
      { "Delete", GLFW_KEY_DELETE },
      { "Right", GLFW_KEY_RIGHT },
      { "Left", GLFW_KEY_LEFT },
      { "Down", GLFW_KEY_DOWN },
      { "Up", GLFW_KEY_UP },
      { "PageUp", GLFW_KEY_PAGE_UP },
      { "PageDown", GLFW_KEY_PAGE_DOWN },
      { "Home", GLFW_KEY_HOME },
      { "End", GLFW_KEY_END },
      { "CapsLock", GLFW_KEY_CAPS_LOCK },
      { "ScrollLock", GLFW_KEY_SCROLL_LOCK },
      { "NumLock", GLFW_KEY_NUM_LOCK },
      { "PrintScreen", GLFW_KEY_PRINT_SCREEN },
      { "Pause", GLFW_KEY_PAUSE },
      { "F1", GLFW_KEY_F1 },
      { "F2", GLFW_KEY_F2 },
      { "F3", GLFW_KEY_F3 },
      { "F4", GLFW_KEY_F4 },
      { "F5", GLFW_KEY_F5 },
      { "F6", GLFW_KEY_F6 },
      { "F7", GLFW_KEY_F7 },
      { "F8", GLFW_KEY_F8 },
      { "F9", GLFW_KEY_F9 },
      { "F10", GLFW_KEY_F10 },
      { "F11", GLFW_KEY_F11 },
      { "F12", GLFW_KEY_F12 },
      { "F13", GLFW_KEY_F13 },
      { "F14", GLFW_KEY_F14 },
      { "F15", GLFW_KEY_F15 },
      { "F16", GLFW_KEY_F16 },
      { "F17", GLFW_KEY_F17 },
      { "F18", GLFW_KEY_F18 },
      { "F19", GLFW_KEY_F19 },
      { "F20", GLFW_KEY_F20 },
      { "F21", GLFW_KEY_F21 },
      { "F22", GLFW_KEY_F22 },
      { "F23", GLFW_KEY_F23 },
      { "F24", GLFW_KEY_F24 },
      { "F25", GLFW_KEY_F25 },
      { "Kp0", GLFW_KEY_KP_0 },
      { "Kp1", GLFW_KEY_KP_1 },
      { "Kp2", GLFW_KEY_KP_2 },
      { "Kp3", GLFW_KEY_KP_3 },
      { "Kp4", GLFW_KEY_KP_4 },
      { "Kp5", GLFW_KEY_KP_5 },
      { "Kp6", GLFW_KEY_KP_6 },
      { "Kp7", GLFW_KEY_KP_7 },
      { "Kp8", GLFW_KEY_KP_8 },
      { "Kp9", GLFW_KEY_KP_9 },
      { "KpDecimal", GLFW_KEY_KP_DECIMAL },
      { "KpDivide", GLFW_KEY_KP_DIVIDE },
      { "KpMultiply", GLFW_KEY_KP_MULTIPLY },
      { "KpSubtract", GLFW_KEY_KP_SUBTRACT },
      { "KpAdd", GLFW_KEY_KP_ADD },
      { "KpEnter", GLFW_KEY_KP_ENTER },
      { "KpEqual", GLFW_KEY_KP_EQUAL },
      { "LeftShift", GLFW_KEY_LEFT_SHIFT },
      { "LeftControl", GLFW_KEY_LEFT_CONTROL },
      { "LeftAlt", GLFW_KEY_LEFT_ALT },
      { "LeftSuper", GLFW_KEY_LEFT_SUPER },
      { "RightShift", GLFW_KEY_RIGHT_SHIFT },
      { "RightControl", GLFW_KEY_RIGHT_CONTROL },
      { "RightAlt", GLFW_KEY_RIGHT_ALT },
      { "RightSuper", GLFW_KEY_RIGHT_SUPER },
      { "Menu", GLFW_KEY_MENU } };
}
