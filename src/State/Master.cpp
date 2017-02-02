#include "Master.hpp"

#include "../OpenGLHeaders.hpp"

#include "../3D/Camera.hpp"
#include "../3D/Cube.hpp"
#include "../3D/Terrain.hpp"
#include "../3D/World.hpp"
#include "../Console/Console.hpp"
#include "../Console/Console.hpp"
#include "../Drawable/Drawable3D.hpp"
#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Lua/Lua.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include <random>

using mmm::vec2;
using mmm::vec3;

Master::Master(Asset* a) {
  CFG*     c         = a->cfg();
  vec2     shadowRes = vec2(c->graphics.shadowRes, c->graphics.shadowRes);
  Console* con       = new Console(a);

  a->rManager()->unloadUnnecessary(ResourceScope::Master);
  a->rManager()->loadRequired(ResourceScope::Master);

  mCamera    = new Camera(a);
  mWorld     = new World(vec3(0, -9.807, 0));
  mShadowmap = new Framebuffer(a->rManager()->get<Program>("Program::Shadow"),
                               shadowRes,
                               true);

  mDrawable3D  = { new Terrain(), new Cube() };
  mGUIElements = { con };

  a->lua()->add(con);

  for (auto d : mDrawable3D)
    mWorld->addObject(d);


  a->lua()->engine.set_function("addCubes", [&](int i) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 5);
    for (int j = 0; j <= i; ++j) {
      vec3 size = vec3(
          mmm::max(dis(gen), 0.0001f),
          mmm::max(dis(gen), 0.0001f),
          mmm::max(dis(gen), 0.0001f));
      log("Creating cube with: ", size, "");
      mDrawable3D.push_back(new Cube(size));
      mWorld->addObject(mDrawable3D.back());
    }
  });

  a->lua()->engine.set_function("clearCubes", [&]() {
    for(unsigned int i = 0; i < mDrawable3D.size(); ++i) {
      if (i != 0) {
        mWorld->removeObject(mDrawable3D[i]);
        delete mDrawable3D[i];
      }
    }

    while(mDrawable3D.size() > 1)
      mDrawable3D.pop_back();
  });
}

Master::~Master() {
  delete mCamera;
  delete mShadowmap;
  delete mWorld;

  for (auto d : mDrawable3D)
    delete d;

  for (auto g : mGUIElements)
    delete g;

  mDrawable3D.clear();
  mGUIElements.clear();
}

void Master::draw3D() {
  glEnable(GL_DEPTH_TEST);

  /* mShadowmap->bind(); */

  /* for (auto d : mDrawable3D) */
  /*   d->draw(mCamera, mDeltaTime); */

  /* mShadowmap->finalize(); */
  mShadowmap->texture()->bind(0);

  for (auto d : mDrawable3D)
    d->draw(mCamera, mDeltaTime);
}

void Master::drawGUI() {
  glDisable(GL_DEPTH_TEST);

  for (auto g : mGUIElements)
    g->draw();
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
}

void Master::update(float deltaTime) {
  mDeltaTime = deltaTime;
  mWorld->doPhysics(deltaTime);

  if (!mGUIElements.back()->isVisible())
    mCamera->input(deltaTime);
  mCamera->update(deltaTime);
}

void Master::draw(float) {
  draw3D();
  drawGUI();
}
