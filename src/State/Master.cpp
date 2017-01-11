#include "Master.hpp"

#include "../OpenGLHeaders.hpp"

#include "../Console/Console.hpp"
#include "../Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

Master::Master(Asset* asset, Input* input) : mInput(input), mAsset(asset) {
  mConsole = new Console(input);
}

Master::~Master() {
  delete mConsole;
}

void Master::draw3D() {
  glEnable(GL_DEPTH_TEST);
}

void Master::drawGUI() {
  glDisable(GL_DEPTH_TEST);
  mConsole->draw(mDeltaTime);
}

void Master::mouseMovementCB(double, double) {}
void Master::mouseScrollCB(double, double) {}
int  Master::mouseButtonCB(int, int, int) {
  return State::NOCHANGE;
}

void Master::charCB(unsigned int) {}

int Master::keyboardCB(int key, int, int action, int mods) {
  int stateChange = State::NOCHANGE;

  if (action == GLFW_RELEASE)
    return stateChange;

  log("Keypress: ", key, " equals: ", Input::keyStrings[key]);

  stateChange = mConsole->handleKeyInput(key, action, mods);

  if (stateChange == State::HANDLED_INPUT) {
    return State::NOCHANGE;
  }

  if (key == GLFW_KEY_ESCAPE) {
    return State::QUIT;
  }

  return stateChange;
}

void Master::update(float deltaTime) {
  mDeltaTime = deltaTime;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0.4, 0.7, 1);
  draw3D();
  drawGUI();
}
