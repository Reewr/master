#include "MainMenu.hpp"

#include "../GUI/Menu.hpp"
#include "../GUI/Window.hpp"
#include "../GUIMenu/OptionsMenu.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Texture.hpp"
#include "../Console/Console.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

MainMenu::MainMenu(Asset* asset) {
  mAsset = asset;

  Console*     console = new Console(asset);
  OptionsMenu* opts = new OptionsMenu(asset->input());
  CFG* cfg          = asset->cfg();
  Window* menu      = new Window("NONE",
                            Rect(vec2(cfg->graphics.res.x - 200,
                                      cfg->graphics.res.y - 225),
                                 vec2(cfg->graphics.res.x,
                                      cfg->graphics.res.y)));
  menu->isVisible(true);
  menu->addMenu("MainMenu",
                { "Master Thesis", "Start Game", "Options", "Exit" },
                { 0, 0 },
                { 20, 50, Menu::VERTICAL, Text::WHITE });


  // Order is important
  mGUIElements = { menu, opts, console };

  auto handler = [&, opts, menu, console](const Input::Event& event) {
    if (!menu->isVisible() || menu->isAnimating())
      return;

    if (event.isAction(Input::Action::Console)) {
      console->isVisible(true);
      return event.stopPropgation();
    }

    menu->defaultInputHandler(event);

    if (event.hasBeenHandled())
      return;

    if (event.keyPressed(GLFW_KEY_ENTER) ||
        event.buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
      switch (menu->menu("MainMenu")->getActiveMenu()) {
        case 0:
          event.sendStateChange(States::MasterThesis);
          event.stopPropgation();
          break;
        case 1:
          event.sendStateChange(States::Game);
          event.stopPropgation();
          break;
        case 2:
          opts->isVisible(true);
          menu->isVisible(false);
          event.stopPropgation();
          break;
        case 3:
          event.sendStateChange(States::Quit);
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

  for (auto i = mGUIElements.rbegin(); i != mGUIElements.rend(); ++i) {
    (*i)->update(deltaTime);
  }
}

void MainMenu::draw(float) {
  draw3D();
  drawGUI();
}

void MainMenu::draw3D() {
  glEnable(GL_DEPTH_TEST);
}

void MainMenu::drawGUI() {
  glDisable(GL_DEPTH_TEST);
  for (auto g : mGUIElements)
    g->draw();
}

void MainMenu::input(const Input::Event& event) {
  for (auto i = mGUIElements.rbegin(); i != mGUIElements.rend(); ++i) {
    (*i)->input(event);

    if (event.hasBeenHandled())
      break;
  }

  switch (event.state()) {
    case States::OptionsMenuClose:
      event.sendStateChange(States::NoChange);
      mGUIElements[1]->isVisible(false);
      mGUIElements[0]->isVisible(true);
    break;
  }
}