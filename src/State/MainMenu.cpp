#include "MainMenu.hpp"

#include "../GUI/Menu.hpp"
#include "../GUI/Window.hpp"
#include "../GUIMenu/OptionsMenu.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

MainMenu::MainMenu(Asset* asset, Input* input) {
  mAsset = asset;
  mInput = input;

  mMenu = new Window("NONE",
                     Rect(vec2(asset->cfg.graphics.res.x - 200,
                               asset->cfg.graphics.res.y - 225),
                          vec2(asset->cfg.graphics.res.x,
                               asset->cfg.graphics.res.y)));
  mMenu->isVisible(true);
  mMenu->addMenu("MainMenu",
                 { "Master Thesis", "Start Game", "Options", "Exit" },
                 { 0, 0 },
                 { 20, 50, Menu::VERTICAL, Text::WHITE });

  mOptionsMenu = new OptionsMenu(input);
  mBackground  = new Window(TEMP::getPath(TEMP::SPACE),
                           Rect(0,
                                0,
                                asset->cfg.graphics.res.x,
                                asset->cfg.graphics.res.y));
  mBackground->isVisible(true);
  log("MainMenu: Initialized successfully...");
}

MainMenu::~MainMenu() {
  delete mMenu;
  delete mOptionsMenu;
  delete mBackground;
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
  mBackground->draw(mDeltaTime);
  mMenu->draw(mDeltaTime);
  mOptionsMenu->draw(mDeltaTime);
}

int MainMenu::handleMenuActionInput() {
  switch (mMenu->menu("MainMenu")->getActiveMenu()) {
    case 0:
      return State::MASTER_THESIS;
    case 1:
      return State::GAME;
    case 2:
      mOptionsMenu->isVisible(true);
      mMenu->isVisible(false);
      break;
    case 3:
      return State::QUIT;
  }
  return State::NOCHANGE;
}

int MainMenu::keyboardCB(int key, int, int action, int) {
  if (action == GLFW_RELEASE)
    return State::NOCHANGE;

  if (mMenu->isVisible()) {
    if (key == GLFW_KEY_ENTER)
      return handleMenuActionInput();
    else
      mMenu->menu("MainMenu")->setActiveMenuKeyboard(key);
  }

  int retOpt = mOptionsMenu->handleKeyInput(key, action);

  if (retOpt == 0)
    mMenu->isVisible(true);
  else if (retOpt == State::REFRESH || retOpt == State::WINREFRESH)
    return retOpt;

  return State::NOCHANGE;
}

void MainMenu::mouseMovementCB(double x, double y) {
  vec2 mosPos(x, y);
  mMenu->setActiveMenuItem("MainMenu", mosPos);
  mOptionsMenu->handleMouseMovement(mosPos);
}

int MainMenu::mouseButtonCB(int button, int action, int) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if (mMenu->isVisible() && !mMenu->isAnimating())
      return handleMenuActionInput();

    int retOpt = mOptionsMenu->handleMouseButton(button, action);
    if (retOpt == 0) {
      mMenu->isVisible(true);
      return State::NOCHANGE;
    } else if (retOpt == State::REFRESH || retOpt == State::WINREFRESH)
      return retOpt;
  }
  return State::NOCHANGE;
}

void MainMenu::mouseScrollCB(double, double) {}