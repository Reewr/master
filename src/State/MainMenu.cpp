#include "MainMenu.hpp"

#include "../GUI/Menu.hpp"
#include "../GUI/Window.hpp"
#include "../GUIMenu/OptionsMenu.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

MainMenu::MainMenu(Asset* asset, Input::Input* input) {
  mAsset = asset;
  mInput = input;

  GUI*    optionsMenu = new OptionsMenu(input);
  Window* menu        = new Window("NONE",
                            Rect(vec2(asset->cfg.graphics.res.x - 200,
                                      asset->cfg.graphics.res.y - 225),
                                 vec2(asset->cfg.graphics.res.x,
                                      asset->cfg.graphics.res.y)));
  menu->isVisible(true);
  menu->addMenu("MainMenu",
                { "Master Thesis", "Start Game", "Options", "Exit" },
                { 0, 0 },
                { 20, 50, Menu::VERTICAL, Text::WHITE });


  GUI* background = new Window(TEMP::getPath(TEMP::SPACE),
                               Rect(0,
                                    0,
                                    asset->cfg.graphics.res.x,
                                    asset->cfg.graphics.res.y));
  background->isVisible(true);

  // Order is important
  mGUIElements = {
    background, menu, optionsMenu,
  };

  auto handler = [&](const Input::Event& event) {
    Window* menu = static_cast<Window*>(mGUIElements[1]);
    GUI*    opts = mGUIElements[2];

    if (!menu->isVisible() || menu->isAnimating())
      return;

    menu->defaultInputHandler(event);

    if (event.hasBeenHandled())
      return;

    if (event.keyPressed(GLFW_KEY_ENTER) ||
        event.buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
      switch (menu->menu("MainMenu")->getActiveMenu()) {
        case 0:
          event.sendStateChange(State::MASTER_THESIS);
          event.stopPropgation();
          break;
        case 1:
          event.sendStateChange(State::GAME);
          event.stopPropgation();
          break;
        case 2:
          opts->isVisible(true);
          menu->isVisible(false);
          event.stopPropgation();
          break;
        case 3:
          event.sendStateChange(State::GAME);
          event.stopPropgation();
          break;
      }
    }
  };

  menu->setInputHandler(handler);
  log("MainMenu: Initialized successfully...");
}

MainMenu::~MainMenu() {
  for (auto g : mGUIElements)
    delete g;
}

void MainMenu::update(float deltaTime) {
  mDeltaTime = deltaTime;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0.4, 0.7, 1);
  draw3D();
  drawGUI();
}

void MainMenu::draw3D() {
  glEnable(GL_DEPTH_TEST);
}

void MainMenu::drawGUI() {
  glDisable(GL_DEPTH_TEST);
  for (auto g : mGUIElements)
    g->draw(mDeltaTime);
}

void MainMenu::input(const Input::Event& event) {
  for (auto i = mGUIElements.rbegin(); i != mGUIElements.rend(); ++i) {
    (*i)->input(event);

    if (event.hasBeenHandled())
      break;
  }

  if (!mGUIElements[1]->isVisible() && !mGUIElements[2]->isVisible()) {
    mGUIElements[1]->isVisible(true);
  }
}