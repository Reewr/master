#include "Master.hpp"

#include "../OpenGLHeaders.hpp"

#include "../3D/Cube.hpp"
#include "../3D/Line.hpp"
#include "../3D/Sphere.hpp"
#include "../3D/Spider.hpp"
#include "../3D/Terrain.hpp"
#include "../3D/World.hpp"
#include "../Camera/Camera.hpp"
#include "../Console/Console.hpp"
#include "../Console/Console.hpp"
#include "../Drawable/Drawable3D.hpp"
#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Lua/Lua.hpp"
#include "../Resource/PhysicsMesh.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include "../Learning/SpiderSwarm.hpp"

#include <random>

using mmm::vec2;
using mmm::vec3;

Master::Master(Asset* a) : mAsset(a) {
  setLoggerName("Master");
  CFG*             c         = a->cfg();
  vec2             shadowRes = vec2(c->graphics.shadowRes);
  ResourceManager* r         = a->rManager();

  r->unloadUnnecessary(ResourceScope::Master);
  r->loadRequired(ResourceScope::Master);

  mLua    = a->lua();
  mCamera = new Camera(a);
  mWorld  = new World(vec3(0, -9.81, 0));
  mShadowmap =
    new Framebuffer(r->get<Program>("Program::Shadow"), shadowRes, true);

  mDrawable3D = { new Terrain() };
  mSwarm      = new SpiderSwarm();
  // mSwarm->load("testswarm");

  for (auto d : mDrawable3D)
    mWorld->addObject(d);

  a->setCamera(mCamera);
  mLua->reInitialize();
  mLua->engine["swarm"] = mSwarm;

  // add CFG is enabled
  if (a->cfg()->console.enabled) {
    mGUIElements.push_back(new Console(a));
  }

  // just run physics once so the terrain is positioned correctly
  mWorld->doPhysics(1);
  mLog->info("Initialized successfully");
}

Master::~Master() {
  delete mCamera;
  delete mShadowmap;
  delete mWorld;
  delete mSwarm;

  for (auto d : mDrawable3D)
    delete d;

  for (auto g : mGUIElements)
    delete g;

  mDrawable3D.clear();
  mGUIElements.clear();
}

void Master::draw3D() {
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);

  std::shared_ptr<Program> shadowProgram = mShadowmap->program();
  /* mCamera->setLightVPUniforms(shadowProgram, "light"); */

  /* mShadowmap->bind(true); */
  /* for (auto d : mDrawable3D) */
  /*   d->draw(shadowProgram, false); */
  /* mSwarm->draw(shadowProgram, false); */
  /* mShadowmap->finalize(); */
  /* mShadowmap->texture()->bind(0); */

  std::shared_ptr<Program> modelProgram =
    mAsset->rManager()->get<Program>("Program::Model");

  modelProgram->setUniform("view", mCamera->view());
  modelProgram->setUniform("proj", mCamera->projection());
  /* modelProgram->setUniform("dir", mCamera->light().direction); */

  /* mCamera->setLightVPUniforms(modelProgram, "light"); */

  for (auto d : mDrawable3D)
    d->draw(modelProgram, true);
  mSwarm->draw(modelProgram, true);
}

void Master::drawGUI() {
  glDisable(GL_DEPTH_TEST);

  for (auto g : mGUIElements)
    g->draw();

  // render shadowMap as a GUI overlay for debugging...
  // mAsset->rManager()->get<Program>("Program::GUI")->bind();
  // mShadowmap->texture()->bind(0);
  // mShadowmap->quad()->draw();

  try {
    mLua->engine["draw"]();
  } catch (const sol::error& e) {
    mLog->error("Failed to draw: {}", e.what());
  }
}

void Master::input(const Input::Event& event) {
  for (auto g : mGUIElements)
    g->input(event);

  if (event.keyPressed(GLFW_KEY_ESCAPE)) {
    event.sendStateChange(States::Quit);
    event.stopPropgation();
  }

  if (event.scrollUp())
    mCamera->zoom(1);
  else if (event.scrollDown())
    mCamera->zoom(-1);

  mWorld->input(mCamera, event);
}

void Master::update(float deltaTime) {
  mDeltaTime = deltaTime;
  // mWorld->doPhysics(deltaTime);
  mSwarm->update(deltaTime);

  if (mGUIElements.size() == 0 || !mGUIElements.back()->isVisible())
    mCamera->input(deltaTime);
  mCamera->update(deltaTime);

  try {
    mLua->engine["update"](deltaTime);
  } catch (const sol::error& e) {
    mLog->error("Failed to update: {}", e.what());
  }
}

void Master::draw(float) {
  draw3D();
  drawGUI();
}
