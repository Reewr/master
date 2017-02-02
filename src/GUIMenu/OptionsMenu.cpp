#include "OptionsMenu.hpp"

#include <algorithm>

#include "../OpenGLHeaders.hpp"

#include "../GUI/Checkbox.hpp"
#include "../GUI/Dropdown.hpp"
#include "../GUI/Inputbox.hpp"
#include "../GUI/Slider.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../State/State.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

using mmm::vec2;

OptionsMenu::OptionsMenu(Input::Input* input) {
  mCFG          = mAsset->cfg();
  mActiveWindow = NULL;
  mInput        = input;
  mBoundingBox  = Rectangle(mCFG->graphics.res.x * 0.50 - 500,
                           mCFG->graphics.res.y * 0.50 - 350,
                           1000,
                           700);

  mBackground = new GLRectangle(mBoundingBox);
  mBackground->setTexture(
    mAsset->rManager()->get<Texture>("Texture::Background"));

  addMenu("Category",
          { "Audio", "Game", "Graphics", "Keybindings", "Mouse" },
          vec2(25, 100));

  menu("Category")
    ->addMenuItem("Accept",
                  { mBoundingBox.middle().x - 200,
                    mBoundingBox.bottomright().y - 50 });

  menu("Category")
    ->addMenuItem("Cancel",
                  { mBoundingBox.middle().x + 100,
                    mBoundingBox.bottomright().y - 50 });

  Rectangle innerRect(vec2(200, 54),
                      vec2(mBoundingBox.size.x - 200,
                           mBoundingBox.size.y - 125));

  addWindow("Audio", innerRect);
  addWindow("Graphics", innerRect);
  addWindow("Keybindings", innerRect);
  addWindow("Mouse", innerRect);
  addWindow("Game", innerRect);

  if (!mUiLoader.loadXMLSettings("./media/XML/GUI.xml", "Options", this))
    throw std::runtime_error("Read log above");

  std::vector<std::string> actions = input->getActionsString();
  window("Keybindings")
    ->addMenu("Keybindings2",
              actions,
              vec2(50, 100),
              { 15, 25, Menu::VERTICAL, Text::WHITE });

  for (unsigned int i = 0; i < actions.size() * 2; i++) {
    vec2 place = vec2(250, 105);
    place += vec2((i % 2 == 0) ? 0 : 250, (i / 2 * 32));
    window("Keybindings")
      ->addInputbox("Input" + Utils::toStr(i),
                    Rectangle(place, vec2(200, 25)),
                    "");
  }

  mInputHandler = [&](const Input::Event& e) {
    if (!isVisible() || isAnimating())
      return;

    if (e == Input::Event::Type::MousePress && !isInside(e.position())) {
      e.sendStateChange(States::OptionsMenuClose);
      e.stopPropgation();
      return;
    }

    defaultInputHandler(e);

    bool isEnterOrEsc =
      e.keyPressed(GLFW_KEY_ENTER) || e.buttonPressed(GLFW_MOUSE_BUTTON_LEFT);

    if (isEnterOrEsc && handleOptionsAction(e)) {
      e.stopPropgation();
      return;
    }

    if (e.keyPressed(GLFW_KEY_ESCAPE)) {
      e.sendStateChange(States::OptionsMenuClose);
      e.stopPropgation();
      return;
    }

    // handle the specific case where an inputbox has changed
    // and overridden some other keybinding. Need to clear that specific
    // keybinding.
    if (e.hasBeenHandled() && e.prevType() == Input::Event::Type::KeyPress) {
      std::string strKey          = Input::glfwKeyToString(e.key());
      std::string inputboxChanged = "";

      for (auto i : window("Keybindings")->inputboxes()) {
        if (i.second->hasChanged())
          inputboxChanged = i.first;
      }

      if (inputboxChanged != "") {
        for (auto i : window("Keybindings")->inputboxes()) {
          if (i.first != inputboxChanged && i.second->text() == strKey)
            i.second->changeText("unbound", true);
        }
      }
    }
  };

  setDefaultOptions();
}

bool OptionsMenu::handleOptionsAction(const Input::Event& event) {
  std::string nextWindow = "";

  switch (menu("Category")->getActiveMenu()) {
    case 0:
      nextWindow = "Audio";
      break;
    case 1:
      nextWindow = "Game";
      break;
    case 2:
      nextWindow = "Graphics";
      break;
    case 3:
      nextWindow = "Keybindings";
      break;
    case 4:
      nextWindow = "Mouse";
      break;
    case 5: {
      isVisible(false);
      int stateChange = parseOptionstoCFG();
      if (stateChange != 0)
        event.sendStateChange(stateChange);
      return true;
    }
    case 6:
      log("Something");
      setDefaultOptions();
      isVisible(false);
      event.sendStateChange(States::OptionsMenuClose);
      return true;
  }

  if (nextWindow != "") {
    if (mActiveWindow != NULL)
      mActiveWindow->isVisible(false);
    addTitle(nextWindow);
    mActiveWindow = window(nextWindow);
    mActiveWindow->isVisible(true);
    return true;
  }

  return false;
}

void OptionsMenu::setDefaultOptions() {
  Window* gr = window("Graphics");
  Window* ga = window("Game");
  Window* kb = window("Keybindings");
  Window* au = window("Audio");

  gr->dropdown("Resolution")
    ->setActiveItem(Utils::toStr((int) mCFG->graphics.res.x) + "x" +
                    Utils::toStr((int) mCFG->graphics.res.y));
  gr->dropdown("Monitor")->setActiveItemIndex(mCFG->graphics.monitor + 1);
  gr->dropdown("Vsync")->setActiveItem(mCFG->getProp("Graphics.vsync"));
  gr->dropdown("DisplayMode")
    ->setActiveItem(mCFG->getProp("Graphics.windowmode"));
  gr->dropdown("Aliasing")
    ->setActiveItem(mCFG->getProp("Graphics.antialiasing"));
  gr->dropdown("Aniso")->setActiveItem(
    mCFG->getProp("Graphics.anisotropicfiltering"));
  gr->dropdown("ShadowSamples")
    ->setActiveItem(mCFG->getProp("Graphics.shadow_samples"));
  gr->dropdown("ShadowRes")
    ->setActiveItem(mCFG->getProp("Graphics.shadow_resolution"));
  gr->dropdown("ViewDistance")
    ->setActiveItem(mCFG->getProp("Graphics.view_distance"));

  ga->dropdown("CamRotSpeed")
    ->setActiveItem(Utils::toStr((int) mCFG->camera.rotSpeed));
  ga->dropdown("CamRotInvH")
    ->setActiveItem(mCFG->getProp("Camera.rotation_inverse_horizontal"));
  ga->dropdown("CamRotInvV")
    ->setActiveItem(mCFG->getProp("Camera.rotation_inverse_vertical"));
  ga->dropdown("CamZoomSpeed")
    ->setActiveItem(Utils::toStr((int) mCFG->camera.zoomSpeed));
  ga->dropdown("CamZoomInv")
    ->setActiveItem(mCFG->getProp("Camera.zoom_inverse"));

  au->slider("SFXVolume")->setSlider(mCFG->audio.SFXVolume);
  au->slider("MusicVolume")->setSlider(mCFG->audio.musicVolume);
  au->slider("MasterVolume")->setSlider(mCFG->audio.masterVolume);

  int action = Input::Action::PauseMenu;
  int index  = 0;

  for (unsigned int i = 0; i < kb->inputboxes().size(); i++) {
    kb->inputboxes()["Input" + Utils::toStr(i)]
      ->changeText(mInput->getActionKeysToString(action)[index], true);
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
  if (graphicalChanged == States::NoChange)
    return 0;
  return graphicalChanged;
}

void OptionsMenu::parseGameOptions() {
  std::map<std::string, std::string> opts;

  for (auto d : window("Game")->dropdowns())
    opts[d.first] = d.second->activeItemText();

  mCFG->setProp("Camera.rotation_speed", { opts["CamRotSpeed"] });
  mCFG->setProp("Camera.rotation_inverse",
                { Utils::toLower(opts["CamRotInv"]) });
  mCFG->setProp("Camera.zoom_speed", { opts["CamZoomSpeed"] });
  mCFG->setProp("Camera.zoom_inverse", { Utils::toLower(opts["CamZoomInv"]) });
}

void OptionsMenu::parseAudioOptions() {
  std::map<std::string, float> opts;

  for (auto s : window("Audio")->sliders())
    opts[s.first] = s.second->value();

  mCFG->audio.SFXVolume    = opts["SFXVolume"];
  mCFG->audio.musicVolume  = opts["MusicVolume"];
  mCFG->audio.masterVolume = opts["MasterVolume"];
}

void OptionsMenu::parseKeybindingOptions() {
  bool hasChanged = false;
  std::map<std::string, std::string> keybinds;

  for (auto s : window("Keybindings")->inputboxes()) {
    keybinds[s.first] = s.second->text();
    hasChanged        = hasChanged || s.second->hasChanged();
  }

  if (!hasChanged)
    return;

  std::vector<std::string> actions = mInput->getActionsString();
  for (unsigned int i = 0; i < actions.size(); i++) {
    std::string s = Utils::toLower(actions[i]);
    std::replace(s.begin(), s.end(), ' ', '_');
    mCFG->setProp("Bindings." + s,
                  { keybinds["Input" + Utils::toStr(i * 2)],
                    keybinds["Input" + Utils::toStr(i * 2 + 1)] });
  }
  mInput->initialize();
}

int OptionsMenu::parseGraphicsOptions() {
  Window* g = window("Graphics");

  std::map<std::string, std::string> opts;
  bool hasChanged = false;
  for (auto d : window("Graphics")->dropdowns()) {
    opts[d.first] = d.second->activeItemText();
    hasChanged    = hasChanged || d.second->hasChanged();
  }

  if (!hasChanged)
    return States::NoChange;

  // Graphical settings being set
  int index = opts["Resolution"].find("x");
  mCFG->setProp("Graphics.resolution.x",
                { opts["Resolution"].substr(0, index) });
  mCFG->setProp("Graphics.resolution.y",
                { opts["Resolution"].substr(index + 1) });
  mCFG->setProp("Graphics.antialiasing", { Utils::toLower(opts["Aliasing"]) });
  mCFG->setProp("Graphics.anisotropicfiltering",
                { Utils::toLower(opts["Aniso"]) });
  mCFG->setProp("Graphics.shadow_samples",
                { Utils::toLower(opts["ShadowSamples"]) });
  mCFG->setProp("Graphics.vsync", { Utils::toLower(opts["Vsync"]) });
  mCFG->setProp("Graphics.windowmode", { Utils::toLower(opts["DisplayMode"]) });
  mCFG->setProp("Graphics.view_distance",
                { Utils::toLower(opts["ViewDistance"]) });
  mCFG->graphics.monitor =
    window("Graphics")->dropdown("Monitor")->activeItemIndex() - 1;
  mCFG->setProp("Graphics.shadow_resolution",
                { Utils::toLower(opts["ShadowRes"]) });

  bool shouldRefresh = g->dropdown("Aliasing")->hasChanged() ||
                       g->dropdown("DisplayMode")->hasChanged();
  return shouldRefresh ? States::WinRefresh : States::Refresh;
}