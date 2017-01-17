#include "Master.hpp"

#include "../OpenGLHeaders.hpp"

#include "../Console/Console.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

Master::Master(Asset*) {}

Master::~Master() {}

void Master::draw3D() {
  glEnable(GL_DEPTH_TEST);
}

void Master::drawGUI() {
  glDisable(GL_DEPTH_TEST);
}

void Master::input(const Input::Event& event) {
  if (event.keyPressed(GLFW_KEY_ESCAPE)) {
    event.sendStateChange(States::Quit);
    event.stopPropgation();
  }
}

void Master::update(float deltaTime) {
  mDeltaTime = deltaTime;
}

void Master::draw(float) {
  draw3D();
  drawGUI();
}
