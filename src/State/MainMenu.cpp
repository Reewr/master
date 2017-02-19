#include "MainMenu.hpp"

#include "../Console/Console.hpp"
#include "../GUI/Menu.hpp"
#include "../GUI/Window.hpp"
#include "../GUIMenu/OptionsMenu.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Lua/Lua.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

using mmm::vec2;

MainMenu::MainMenu(Asset* asset) {
  setLoggerName("MainMenu");
  mAsset = asset;
  mLua   = mAsset->lua();
  mAsset->rManager()->unloadUnnecessary(ResourceScope::MainMenu);
  mAsset->rManager()->loadRequired(ResourceScope::MainMenu);

  OptionsMenu* opts = new OptionsMenu(mAsset->input());
  CFG*         cfg  = mAsset->cfg();
  Window*      menu =
    new Window("NONE",
               Rectangle(vec2(cfg->graphics.res.x - 175,
                              cfg->graphics.res.y - 175),
                         vec2(cfg->graphics.res.x, cfg->graphics.res.y)));
  menu->isVisible(true);
  menu->addMenu("MainMenu",
                { "Master Thesis", "Start Game", "Options", "Exit" },
                { 0, 0 },
                { 15, 50, Menu::VERTICAL, Text::WHITE });


  // Order is important
  mGUIElements = { menu, opts };

  auto handler = [&, opts, menu](const Input::Event& event) {
    if (!menu->isVisible() || menu->isAnimating())
      return;

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

  // add CFG is enabled
  if (cfg->console.enabled) {
    Console* console = new Console(mAsset);
    mLua->add(console);
    mGUIElements.push_back(console);
  }

  menu->setInputHandler(handler);
  mLua->loadFile("lua/states/mainmenu.lua");

  /* mLua->on("reInitialize", [&](){ */
  /*   mLua->add(console); */
  /*   mLua->loadFile("lua/states/mainmenu.lua"); */
  /* }); */
  mLog->debug("Initialized successfully...");
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

  try {
    mLua->engine["update"](deltaTime);
  } catch (const sol::error& e) {
    mLog->error("Failed to update: ", e.what());
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

  try {
    mLua->engine["draw"]();
  } catch (const sol::error& e) {
    mLog->error("Failed to draw: ", e.what());
  }
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
