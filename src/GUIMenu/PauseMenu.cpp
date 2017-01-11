#include "PauseMenu.hpp"

#include "../OpenGLHeaders.hpp"

#include "../Graphical/Texture.hpp"
#include "../Input.hpp"
#include "../State/State.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

PauseMenu::PauseMenu(Input* i) {
  mBoundingBox = Rect(mCFG->graphics.res.x * 0.5 - 300,
                      mCFG->graphics.res.y * 0.5 - 100,
                      600,
                      200);

  mInput = i;
  mTex   = new Texture(TEMP::getPath(TEMP::OPTSMENU));
  mTex->recalculateGeometry(mBoundingBox);

  if (!mUiLoader.loadXMLSettings(TEMP::getPath(TEMP::XMLOPT), "Pause", this))
    throw Error("Read log above");
}

int PauseMenu::handleKeyInput(const int key, const int action) {
  if (isAnimating() || action != GLFW_PRESS)
    return State::NOCHANGE;

  bool isPauseMenuKey = mInput->checkKey(Input::PAUSEMENU, key);
  bool isEnterKey     = GLFW_KEY_ENTER == key;

  isVisible(isPauseMenuKey && !isVisible());

  if (isEnterKey && isVisible())
    return handleAction();

  if (!isPauseMenuKey && !isEnterKey)
    menu("PauseOptions")->setActiveMenuKeyboard(key);

  return State::NOCHANGE;
}

int PauseMenu::handleMouseButton(const int key, const int) {
  if (isVisible() && !isAnimating() && key == GLFW_MOUSE_BUTTON_1)
    return handleAction();
  return State::NOCHANGE;
}

int PauseMenu::handleAction() {
  switch (menu("PauseOptions")->getActiveMenu()) {
    case 0:
      isVisible(false);
      break;
    case 1:
      isVisible(false);
      return 0;
    case 2:
      return State::QUIT;
  }
  return State::NOCHANGE;
}