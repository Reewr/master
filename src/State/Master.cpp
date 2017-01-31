#include "Master.hpp"

#include "../OpenGLHeaders.hpp"

#include "../Drawable/Drawable3D.hpp"
#include "../Resource/Texture.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Console/Console.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/CFG.hpp"
#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../3D/Camera.hpp"
#include "../3D/Terrain.hpp"
#include "../3D/Cube.hpp"
#include "../3D/World.hpp"

using mmm::vec2;
using mmm::vec3;

Master::Master(Asset* a) {
  CFG* c         = a->cfg();
  vec2 shadowRes = vec2(c->graphics.shadowRes, c->graphics.shadowRes);

  mCamera    = new Camera(a);
  mWorld     = new World(vec3(0, 1, 0));
  mShadowmap = new Framebuffer(a->rManager()->get<Program>("Program::Shadow"),
                               shadowRes,
                               true);

  mDrawable3D = { new Terrain(), new Cube() };

  for (auto d : mDrawable3D)
    mWorld->addObject(d);
}

Master::~Master() {
  delete mCamera;
  delete mShadowmap;
  delete mWorld;

  for(auto d : mDrawable3D)
    delete d;

  mDrawable3D.clear();
}

void Master::draw3D() {
  glEnable(GL_DEPTH_TEST);

  mShadowmap->bind();

  for (auto d : mDrawable3D)
    d->draw(mCamera, mDeltaTime);

  mShadowmap->finalize();
  mShadowmap->texture()->bind(0);

  for (auto d : mDrawable3D)
    d->draw(mCamera, mDeltaTime);

}

void Master::drawGUI() {
  glDisable(GL_DEPTH_TEST);
}

void Master::input(const Input::Event& event) {
  if (event.keyPressed(GLFW_KEY_ESCAPE)) {
    event.sendStateChange(States::Quit);
    event.stopPropgation();
  }

  if (event.isKeyEvent())
    mCamera->input(event, mDeltaTime);
}

void Master::update(float deltaTime) {
  mDeltaTime = deltaTime;
  mWorld->doPhysics(deltaTime);
  mCamera->update(deltaTime);
}

void Master::draw(float) {
  draw3D();
  drawGUI();
}
