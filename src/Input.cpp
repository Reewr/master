#include "Input.hpp"

#include "OpenGLHeaders.hpp"

#include "Utils/Asset.hpp"

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
Keys Input::getKey(int action) {
  if (keys.count(action))
    return keys[action];
  return { GLFW_KEY_UNKNOWN, GLFW_KEY_UNKNOWN };
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
      keys[glfwKeys[glfwKey]].key1 = NOT_BOUND;
    else if (keys[glfwKeys[glfwKey]].key2 == glfwKey)
      keys[glfwKeys[glfwKey]].key2 = NOT_BOUND;
  }
  glfwKeys[glfwKey] = NOT_BOUND;
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
 *   May return NOT_BOUND which means there are no actions
 *   bound to the given key.
 *
 * @param glfwKey
 *
 * @return
 */
int Input::getAction(int glfwKey) {
  if (glfwKeys.count(glfwKey))
    return glfwKeys[glfwKey];
  return NOT_BOUND;
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
vec2 Input::getPressedCoord(int btn) {
  if (!pressedCoords.count(btn))
    return vec2();
  return pressedCoords.at(btn);
}

/**
 * @brief
 *   Get the names of all the actions
 *
 * @return
 */
std::vector<std::string> Input::getActionsString() {
  return actionsString;
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
std::string Input::glfwKeyToString(int glfwKey) {
  if (keyStrings.count(glfwKey))
    return keyStrings[glfwKey];
  return "unbound";
}

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
 *   This lets you store a function that should be executed whenever
 *   an key is clicked. This function can then be executed by using the
 *   doKeyCB() function.
 *
 * @param key
 * @param f
 * @param isPressed
 */
void Input::addKeyCB(int key, std::function<int(void)>&& f, int isPressed) {
  functionCalls[key] = { isPressed, f };
}

/**
 * @brief
 *   Executes a function that was previously stored by an addKeyCB call.
 *   The glfwAction here is whether the key was pressed or not.
 *
 * @param key
 * @param action
 *
 * @return
 */
int Input::doKeyCB(int key, int glfwAction) {
  if (functionCalls.count(key) && functionCalls[key].isPressed == glfwAction)
    return functionCalls[key].func();
  return 0;
}

/**
 * @brief Clears the functions stored using addKeyCB()
 */
void Input::resetKeyCB() {
  functionCalls.clear();
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
  addAction(PAUSEMENU, b->pauseMenu, "Pause Menu");
  addAction(MOVE_UP, b->moveUp, "Move up");
  addAction(MOVE_DOWN, b->moveDown, "Move down");
  addAction(MOVE_LEFT, b->moveLeft, "Move left");
  addAction(MOVE_RIGHT, b->moveRight, "Move right");
  addAction(ROTATE, { GLFW_MOUSE_BUTTON_3 }, "Rotate");
  addAction(SCREENSHOT, b->screenshot, "Take screenshot");
  addAction(HIDEGUI, b->hideGUI, "Hide GUI");
  addAction(CONSOLE, b->showConsole, "Show console");
}

std::map<int, std::string> Input::keyStrings = { { -1, "unbound" },
                                                 { 0, "mousebutton1" },
                                                 { 1, "mousebutton2" },
                                                 { 2, "mousebutton3" },
                                                 { 32, "space" },
                                                 { 39, "apostrophe" },
                                                 { 44, "comma" },
                                                 { 45, "minus" },
                                                 { 46, "." },
                                                 { 47, "slash" },
                                                 { 48, "0" },
                                                 { 49, "1" },
                                                 { 50, "2" },
                                                 { 51, "3" },
                                                 { 52, "4" },
                                                 { 53, "5" },
                                                 { 54, "6" },
                                                 { 55, "7" },
                                                 { 56, "8" },
                                                 { 57, "9" },
                                                 { 59, "semicolon" },
                                                 { 61, "equal" },
                                                 { 65, "a" },
                                                 { 66, "b" },
                                                 { 67, "c" },
                                                 { 68, "d" },
                                                 { 69, "e" },
                                                 { 70, "f" },
                                                 { 71, "g" },
                                                 { 72, "h" },
                                                 { 73, "i" },
                                                 { 74, "j" },
                                                 { 75, "k" },
                                                 { 76, "l" },
                                                 { 77, "m" },
                                                 { 78, "n" },
                                                 { 79, "o" },
                                                 { 80, "p" },
                                                 { 81, "q" },
                                                 { 82, "r" },
                                                 { 83, "s" },
                                                 { 84, "t" },
                                                 { 85, "u" },
                                                 { 86, "v" },
                                                 { 87, "w" },
                                                 { 88, "x" },
                                                 { 89, "y" },
                                                 { 90, "z" },
                                                 { 91, "left_bracket" },
                                                 { 92, "|" },
                                                 { 93, "right_bracket" },
                                                 { 96, "grave_accent" },
                                                 { 256, "escape" },
                                                 { 257, "enter" },
                                                 { 258, "tab" },
                                                 { 259, "backspace" },
                                                 { 260, "insert" },
                                                 { 261, "delete" },
                                                 { 262, "right" },
                                                 { 263, "left" },
                                                 { 264, "down" },
                                                 { 265, "up" },
                                                 { 266, "page_up" },
                                                 { 267, "page_down" },
                                                 { 268, "home" },
                                                 { 269, "end" },
                                                 { 280, "caps_lock" },
                                                 { 281, "scroll_lock" },
                                                 { 282, "num_lock" },
                                                 { 283, "print_screen" },
                                                 { 284, "pause" },
                                                 { 290, "f1" },
                                                 { 291, "f2" },
                                                 { 292, "f3" },
                                                 { 293, "f4" },
                                                 { 294, "f5" },
                                                 { 295, "f6" },
                                                 { 296, "f7" },
                                                 { 297, "f8" },
                                                 { 298, "f9" },
                                                 { 299, "f10" },
                                                 { 300, "f11" },
                                                 { 301, "f12" },
                                                 { 302, "f13" },
                                                 { 303, "f14" },
                                                 { 304, "f15" },
                                                 { 305, "f16" },
                                                 { 306, "f17" },
                                                 { 307, "f18" },
                                                 { 308, "f19" },
                                                 { 309, "f20" },
                                                 { 310, "f21" },
                                                 { 311, "f22" },
                                                 { 312, "f23" },
                                                 { 313, "f24" },
                                                 { 314, "f25" },
                                                 { 320, "kp_0" },
                                                 { 321, "kp_1" },
                                                 { 322, "kp_2" },
                                                 { 323, "kp_3" },
                                                 { 324, "kp_4" },
                                                 { 325, "kp_5" },
                                                 { 326, "kp_6" },
                                                 { 327, "kp_7" },
                                                 { 328, "kp_8" },
                                                 { 329, "kp_9" },
                                                 { 330, "kp_decimal" },
                                                 { 331, "kp_divide" },
                                                 { 332, "kp_multiply" },
                                                 { 333, "kp_subtract" },
                                                 { 334, "kp_add" },
                                                 { 335, "kp_enter" },
                                                 { 336, "kp_equal" },
                                                 { 340, "left_shift" },
                                                 { 341, "left_control" },
                                                 { 342, "left_alt" },
                                                 { 343, "left_super" },
                                                 { 344, "right_shift" },
                                                 { 345, "right_control" },
                                                 { 346, "right_alt" },
                                                 { 347, "right_super" },
                                                 { 348, "menu" } };

std::map<std::string, int> Input::keyMap = { { "unbound", -1 },
                                             { "mousebutton1", 0 },
                                             { "mousebutton2", 1 },
                                             { "mousebutton3", 2 },
                                             { "space", 32 },
                                             { "apostrophe", 39 },
                                             { "comma", 44 },
                                             { "minus", 45 },
                                             { ".", 46 },
                                             { "slash", 47 },
                                             { "0", 48 },
                                             { "1", 49 },
                                             { "2", 50 },
                                             { "3", 51 },
                                             { "4", 52 },
                                             { "5", 53 },
                                             { "6", 54 },
                                             { "7", 55 },
                                             { "8", 56 },
                                             { "9", 57 },
                                             { "semicolon", 59 },
                                             { "equal", 61 },
                                             { "a", 65 },
                                             { "b", 66 },
                                             { "c", 67 },
                                             { "d", 68 },
                                             { "e", 69 },
                                             { "f", 70 },
                                             { "g", 71 },
                                             { "h", 72 },
                                             { "i", 73 },
                                             { "j", 74 },
                                             { "k", 75 },
                                             { "l", 76 },
                                             { "m", 77 },
                                             { "n", 78 },
                                             { "o", 79 },
                                             { "p", 80 },
                                             { "q", 81 },
                                             { "r", 82 },
                                             { "s", 83 },
                                             { "t", 84 },
                                             { "u", 85 },
                                             { "v", 86 },
                                             { "w", 87 },
                                             { "x", 88 },
                                             { "y", 89 },
                                             { "z", 90 },
                                             { "left_bracket", 91 },
                                             { "|", 92 },
                                             { "right_bracket", 93 },
                                             { "grave_accent", 96 },
                                             { "escape", 256 },
                                             { "enter", 257 },
                                             { "tab", 258 },
                                             { "backspace", 259 },
                                             { "insert", 260 },
                                             { "delete", 261 },
                                             { "right", 262 },
                                             { "left", 263 },
                                             { "down", 264 },
                                             { "up", 265 },
                                             { "page_up", 266 },
                                             { "page_down", 267 },
                                             { "home", 268 },
                                             { "end", 269 },
                                             { "caps_lock", 280 },
                                             { "scroll_lock", 281 },
                                             { "num_lock", 282 },
                                             { "print_screen", 283 },
                                             { "pause", 284 },
                                             { "f1", 290 },
                                             { "f2", 291 },
                                             { "f3", 292 },
                                             { "f4", 293 },
                                             { "f5", 294 },
                                             { "f6", 295 },
                                             { "f7", 296 },
                                             { "f8", 297 },
                                             { "f9", 298 },
                                             { "f10", 299 },
                                             { "f11", 300 },
                                             { "f12", 301 },
                                             { "f13", 302 },
                                             { "f14", 303 },
                                             { "f15", 304 },
                                             { "f16", 305 },
                                             { "f17", 306 },
                                             { "f18", 307 },
                                             { "f19", 308 },
                                             { "f20", 309 },
                                             { "f21", 310 },
                                             { "f22", 311 },
                                             { "f23", 312 },
                                             { "f24", 313 },
                                             { "f25", 314 },
                                             { "kp_0", 320 },
                                             { "kp_1", 321 },
                                             { "kp_2", 322 },
                                             { "kp_3", 323 },
                                             { "kp_4", 324 },
                                             { "kp_5", 325 },
                                             { "kp_6", 326 },
                                             { "kp_7", 327 },
                                             { "kp_8", 328 },
                                             { "kp_9", 329 },
                                             { "kp_decimal", 330 },
                                             { "kp_divide", 331 },
                                             { "kp_multiply", 332 },
                                             { "kp_subtract", 333 },
                                             { "kp_add", 334 },
                                             { "kp_enter", 335 },
                                             { "kp_equal", 336 },
                                             { "left_shift", 340 },
                                             { "left_control", 341 },
                                             { "left_alt", 342 },
                                             { "left_super", 343 },
                                             { "right_shift", 344 },
                                             { "right_control", 345 },
                                             { "right_alt", 346 },
                                             { "right_super", 347 },
                                             { "menu", 348 } };
