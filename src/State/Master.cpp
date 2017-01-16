#include "Master.hpp"

#include "../OpenGLHeaders.hpp"

#include "../Console/Console.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

Master::Master(Asset*, Input::Input* input) {
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

void Master::input(const Input::Event& event) {
  mConsole->input(event);

  if (event.keyPressed(GLFW_KEY_ESCAPE)) {
    event.sendStateChange(State::QUIT);
    event.stopPropgation();
  }
}

void Master::update(float deltaTime) {
  mDeltaTime = deltaTime;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0.4, 0.7, 1);
  draw3D();
  drawGUI();
}
