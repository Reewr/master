#include "OptionsMenu.hpp"

#include <algorithm>

#include "../OpenGLHeaders.hpp"

#include "../GUI/Inputbox.hpp"
#include "../GUI/Slider.hpp"
#include "../GUI/Dropdown.hpp"
#include "../GUI/Checkbox.hpp"
#include "../Graphical/Texture.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"
#include "../Input.hpp"
#include "../State/State.hpp"


OptionsMenu::OptionsMenu(Input* input) {
  mActiveWindow = NULL;
  mInput = input;
  mBoundingBox = Rect(
    mCFG->graphics.res.x * 0.50 - 500,
    mCFG->graphics.res.y * 0.50 - 350,
    1000, 700
  );

  mTex = new Texture(TEMP::getPath(TEMP::OPTSMENU));
  mTex->recalculateGeometry(mBoundingBox);

  addMenu("Category", {
    "Audio", "Game", "Graphics", "Keybindings", "Mouse"
    }, vec2(25, 100));

  menu("Category")->addMenuItem(
      "Accept",
      {mBoundingBox.middle().x - 200, mBoundingBox.bottomright().y-50});

  menu("Category")->addMenuItem(
      "Cancel",
      {mBoundingBox.middle().x + 100, mBoundingBox.bottomright().y-50});

  Rect innerRect(vec2(200, 54),
                 vec2(mBoundingBox.size.x - 200, mBoundingBox.size.y - 125));

  addWindow("Audio", innerRect);
  addWindow("Graphics", innerRect);
  addWindow("Keybindings", innerRect);
  addWindow("Mouse", innerRect);
  addWindow("Game", innerRect);

  if (!mUiLoader.loadXMLSettings(TEMP::getPath(TEMP::XMLOPT), "Options", this))
    throw Error("Read log above");

  std::vector<std::string> actions = input->getActionsString();
  window("Keybindings")->addMenu(
    "Keybindings2", actions, vec2(50, 100), {15, 25, Menu::VERTICAL, Text::WHITE
  });

  for(unsigned int i = 0; i < actions.size()*2; i++) {
    vec2 place = vec2(250, 105);
    place += vec2((i % 2 == 0) ? 0 : 250, (i / 2 * 32));
    window("Keybindings")->addInputbox(
      "Input" + Utils::toStr(i), Rect(place, vec2(200, 25)), ""
    );
  }

  setDefaultOptions();
}

bool OptionsMenu::hasChanged() {
  for(auto w : windows()) {
    for(auto d : w.second->dropdowns()) {
      if(d.second->hasChanged())
        return true;
    }
    for(auto i : w.second->inputboxes()) {
      if(i.second->hasChanged())
        return true;
    }
    for(auto s : w.second->sliders()) {
      if(s.second->hasChanged())
        return true;
    }
    for(auto c : w.second->checkboxes()) {
      if(c.second->hasChanged())
        return true;
    }
  }
  return false;
}

void OptionsMenu::hasChanged(bool c) {
  for(auto w : windows()) {

    for(auto d : w.second->dropdowns())
      d.second->hasChanged(c);

    for(auto i : w.second->inputboxes())
      i.second->hasChanged(c);

    for(auto s : w.second->sliders())
      s.second->hasChanged(c);

    for(auto ch : w.second->checkboxes())
      ch.second->hasChanged(c);
  }
}

int OptionsMenu::handleKeyInput(const int key, const int action) {
  if (!isVisible() || isAnimating() || action != GLFW_PRESS)
    return State::NOCHANGE;

  if (key == GLFW_KEY_ENTER)
    return handleAction();

  if (key == GLFW_KEY_ESCAPE) {
    isVisible(false);
    return 0;
  }

  if (window("Keybindings")->isVisible()) {
    std::string strKey = mInput->glfwKeyToString(key);
    std::string inputboxChanged = "";

    for(auto i : window("Keybindings")->inputboxes()) {
      if(i.second->changeText(strKey))
        inputboxChanged = i.first;
    }

    if(inputboxChanged != "") {
      for(auto i : window("Keybindings")->inputboxes()) {
        if(i.first != inputboxChanged && i.second->text() == strKey)
          i.second->changeText("unbound", true);
      }
    }
  } else
    menu("Category")->setActiveMenuKeyboard(key);

  return State::NOCHANGE;
}

int OptionsMenu::handleMouseButton(const int, const int action) {
  if (!isVisible() || isAnimating() || action != GLFW_PRESS)
    return State::NOCHANGE;

  vec2 mosPos = mInput->getMouseCoords();

  if (!isInside(mosPos)) {
    isVisible(false);
    return 0;
  }

  setActiveMenuItem("Category", mosPos);
  return handleAction();
}

void OptionsMenu::handleMouseMovement(const vec2& pos) {
  setActiveMenuItem("Category", pos);

  if (window("Audio")->isVisible()) {
    for (auto s : window("Audio")->sliders())
      if (mInput->isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
        s.second->moveSlider(pos);
  }

  if (window("Graphics")->isVisible())
    for (auto d : window("Graphics")->dropdowns())
      d.second->setMouseOverItem(d.second->isInsideDropItem(pos));

  if (window("Game")->isVisible())
    for (auto d : window("Game")->dropdowns())
      d.second->setMouseOverItem(d.second->isInsideDropItem(pos));
}

int OptionsMenu::handleAction() {
  vec2 mosPos = mInput->getMouseCoords();
  std::string nextWindow = "";

  switch (menu("Category")->getActiveMenu()) {
    case 0: nextWindow = "Audio";       break;
    case 1: nextWindow = "Game";        break;
    case 2: nextWindow = "Graphics";    break;
    case 3: nextWindow = "Keybindings"; break;
    case 4: nextWindow = "Mouse";       break;
    case 5:
      isVisible(false);
      return parseOptionstoCFG();
    case 6:
      setDefaultOptions();
      isVisible(false);
      return 0;
  }

  if (nextWindow != "") {
    if (mActiveWindow != NULL)
      mActiveWindow->isVisible(false);
    addTitle(nextWindow);
    mActiveWindow = window(nextWindow);
  }

  if (mActiveWindow != NULL) {
    mActiveWindow->isVisible(true);
    for (auto d : mActiveWindow->dropdowns())
      d.second->setActiveItem(mosPos);
    for (auto s : mActiveWindow->sliders())
      s.second->moveSlider(mosPos);
    for (auto i : mActiveWindow->inputboxes())
      i.second->showInputbox(mosPos);
  }

  return State::NOCHANGE;
}

void OptionsMenu::setDefaultOptions() {
  Window* gr = window("Graphics");
  Window* ga = window("Game");
  Window* kb = window("Keybindings");
  Window* au = window("Audio");

  gr->dropdown("Resolution")->setActiveItem(
    Utils::toStr((int)mCFG->graphics.res.x)
    + "x" +
    Utils::toStr((int)mCFG->graphics.res.y)
  );
  gr->dropdown("Monitor")->setActiveItemIndex(mCFG->graphics.monitor+1);
  gr->dropdown("Vsync")->setActiveItem(mCFG->getProp("Graphics.vsync"));
  gr->dropdown("DisplayMode")->setActiveItem(mCFG->getProp("Graphics.windowmode"));
  gr->dropdown("Aliasing")->setActiveItem(mCFG->getProp("Graphics.antialiasing"));
  gr->dropdown("Aniso")->setActiveItem(mCFG->getProp("Graphics.anisotropicfiltering"));
  gr->dropdown("ShadowSamples")->setActiveItem(mCFG->getProp("Graphics.shadow_samples"));
  gr->dropdown("ShadowRes")->setActiveItem(mCFG->getProp("Graphics.shadow_resolution"));
  gr->dropdown("ViewDistance")->setActiveItem(mCFG->getProp("Graphics.view_distance"));

  ga->dropdown("CamRotSpeed")->setActiveItem(Utils::toStr((int)mCFG->camera.rotSpeed));
  ga->dropdown("CamRotInvH")->setActiveItem(mCFG->getProp("Camera.rotation_inverse_horizontal"));
  ga->dropdown("CamRotInvV")->setActiveItem(mCFG->getProp("Camera.rotation_inverse_vertical"));
  ga->dropdown("CamZoomSpeed")->setActiveItem(Utils::toStr((int)mCFG->camera.zoomSpeed));
  ga->dropdown("CamZoomInv")->setActiveItem(mCFG->getProp("Camera.zoom_inverse"));

  au->slider("SFXVolume")->setSlider(mCFG->audio.SFXVolume);
  au->slider("MusicVolume")->setSlider(mCFG->audio.musicVolume);
  au->slider("MasterVolume")->setSlider(mCFG->audio.masterVolume);

  int action = Input::PAUSEMENU;
  int index = 0;

  for (unsigned int i = 0; i < kb->inputboxes().size(); i++) {
    kb->inputboxes()["Input" + Utils::toStr(i)]->changeText(
      mInput->getActionKeysToString(action)[index], true
    );
    index = (index == 1) ? 0 : 1;
    action += (index == 0) ? 1 : 0;
  }

  hasChanged(false);
}

int OptionsMenu::parseOptionstoCFG() {
  int graphicalChanged = parseGraphicsOptions();
  parseGameOptions();
  parseAudioOptions();
  parseKeybindingOptions();
  hasChanged(false);
  if (graphicalChanged == State::NOCHANGE)
    return 0;
  return graphicalChanged;
}

void OptionsMenu::parseGameOptions() {
  std::map<std::string, std::string> opts;

  for (auto d : window("Game")->dropdowns())
    opts[d.first] = d.second->activeItemText();

  mCFG->setProp("Camera.rotation_speed", {opts["CamRotSpeed"]});
  mCFG->setProp("Camera.rotation_inverse", {Utils::toLower(opts["CamRotInv"])});
  mCFG->setProp("Camera.zoom_speed", {opts["CamZoomSpeed"]});
  mCFG->setProp("Camera.zoom_inverse", {Utils::toLower(opts["CamZoomInv"])});
}

void OptionsMenu::parseAudioOptions() {
  std::map<std::string, float> opts;

  for (auto s : window("Audio")->sliders())
    opts[s.first] = s.second->value();

  mCFG->audio.SFXVolume = opts["SFXVolume"];
  mCFG->audio.musicVolume = opts["MusicVolume"];
  mCFG->audio.masterVolume = opts["MasterVolume"];
}

void OptionsMenu::parseKeybindingOptions() {
  bool hasChanged = false;
  std::map<std::string, std::string> keybinds;

  for (auto s : window("Keybindings")->inputboxes()) {
    keybinds[s.first] = s.second->text();
    hasChanged = hasChanged || s.second->hasChanged();
  }

  if (!hasChanged)
    return;

  std::vector<std::string> actions = mInput->getActionsString();
  for (unsigned int i = 0; i < actions.size(); i++) {
    std::string s = Utils::toLower(actions[i]);
    std::replace(s.begin(), s.end(), ' ', '_');
    mCFG->setProp("Bindings."+s, {
      keybinds["Input"+Utils::toStr(i*2)], keybinds["Input"+Utils::toStr(i*2+1)]
    });
  }
  mInput->initialize();
}

int OptionsMenu::parseGraphicsOptions() {
  Window* g = window("Graphics");

  std::map<std::string, std::string> opts;
  bool hasChanged = false;
  for (auto d : window("Graphics")->dropdowns()) {
    opts[d.first] = d.second->activeItemText();
    hasChanged = hasChanged || d.second->hasChanged();
  }

  if (!hasChanged)
    return State::NOCHANGE;

  // Graphical settings being set
  int index = opts["Resolution"].find("x");
  mCFG->setProp("Graphics.resolution.x", {opts["Resolution"].substr(0, index)});
  mCFG->setProp("Graphics.resolution.y", {opts["Resolution"].substr(index+1)});
  mCFG->setProp("Graphics.antialiasing", {Utils::toLower(opts["Aliasing"])});
  mCFG->setProp("Graphics.anisotropicfiltering", {Utils::toLower(opts["Aniso"])});
  mCFG->setProp("Graphics.shadow_samples", {Utils::toLower(opts["ShadowSamples"])});
  mCFG->setProp("Graphics.vsync", {Utils::toLower(opts["Vsync"])});
  mCFG->setProp("Graphics.windowmode", {Utils::toLower(opts["DisplayMode"])});
  mCFG->setProp("Graphics.view_distance", {Utils::toLower(opts["ViewDistance"])});
  mCFG->graphics.monitor = window("Graphics")->dropdown("Monitor")->activeItemIndex()-1;
  mCFG->setProp("Graphics.shadow_resolution", {Utils::toLower(opts["ShadowRes"])});

  bool shouldRefresh = g->dropdown("Aliasing")->hasChanged() ||
                       g->dropdown("DisplayMode")->hasChanged();
  return shouldRefresh ? State::WINREFRESH : State::REFRESH;
}