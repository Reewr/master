#include "Master.hpp"

#include "../Input.hpp"
#include "../Utils/Asset.hpp"
#include "../GUIMenu/Console.hpp"

Master::Master(Asset* asset, Input* input) : mInput(input), mAsset(asset) {
  mConsole = new GConsole(input);
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

void Master::mouseMovementCB(double, double) { }
void Master::mouseScrollCB(double, double) { }
int Master::mouseButtonCB(int, int, int) { }
int Master::keyboardCB(int key, int, int action, int mods) {
  int stateChange = State::NOCHANGE;

  if (action == GLFW_RELEASE)
    return stateChange;

  if (mConsole->isVisible()) {
    return mConsole->handleKeyInput(key, action, mods);
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
