#include "PauseMenu.hpp"

#include "../OpenGLHeaders.hpp"

#include "../Import/UILoader.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../State/State.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

PauseMenu::PauseMenu(Input::Input* i) {
  setLoggerName("PauseMenu");
  mBoundingBox = Rectangle(mAsset->cfg()->graphics.res.x * 0.5 - 300,
                           mAsset->cfg()->graphics.res.y * 0.5 - 100,
                           600,
                           200);

  mInput      = i;
  mBackground = new GLRectangle(mBoundingBox);
  mBackground->setTexture(
    mAsset->rManager()->get<Texture>("Texture::Background"));

  Import::UILoader uiLoader;
  if (!uiLoader.loadXMLSettings("./media/XML/GUI.xml", "Pause", this))
    throw std::runtime_error("Read log above");
}

void PauseMenu::input(const Input::Event& event) {
  if (mInput->checkKey(Input::Action::PauseMenu, event.key())) {
    isVisible(!isVisible());
    return event.stopPropgation();
  }

  if (!isVisible() || isAnimating())
    return;

  bool isClick = event.keyPressed(GLFW_KEY_ENTER) ||
                 event.buttonPressed(GLFW_MOUSE_BUTTON_LEFT);
  if (isClick) {
    switch (menu("PauseOptions")->getActiveMenu()) {
      case 0:
        isVisible(false);
        return event.stopPropgation();
      case 1:
        isVisible(false);
        return event.stopPropgation();
      case 2:
        event.sendStateChange(States::Quit);
        return event.stopPropgation();
    }
  }

  defaultInputHandler(event);
}

/* int PauseMenu::handleKeyInput(const int key, const int action) { */
/*   if (isAnimating() || action != GLFW_PRESS) */
/*     return State::NOCHANGE; */

/*   bool isPauseMenuKey = mInput->checkKey(Input::Action::PauseMenu, key); */
/*   bool isEnterKey     = GLFW_KEY_ENTER == key; */

/*   isVisible(isPauseMenuKey && !isVisible()); */

/*   if (isEnterKey && isVisible()) */
/*     return handleAction(); */

/*   if (!isPauseMenuKey && !isEnterKey) */
/*     menu("PauseOptions")->setActiveMenuKeyboard(key); */

/*   return State::NOCHANGE; */
/* } */

/* int PauseMenu::handleMouseButton(const int key, const int) { */
/*   if (isVisible() && !isAnimating() && key == GLFW_MOUSE_BUTTON_1) */
/*     return handleAction(); */
/*   return State::NOCHANGE; */
/* } */

/* int PauseMenu::handleAction() { */
/*   switch (menu("PauseOptions")->getActiveMenu()) { */
/*     case 0: */
/*       isVisible(false); */
/*       break; */
/*     case 1: */
/*       isVisible(false); */
/*       return 0; */
/*     case 2: */
/*       return State::QUIT; */
/*   } */
/*   return State::NOCHANGE; */
/* } */
