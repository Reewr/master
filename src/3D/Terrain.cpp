#include "Terrain.hpp"

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "Camera.hpp"
#include "GLSL/Program.hpp"
#include "Graphical/Framebuffer.hpp"
#include "Resource/ResourceManager.hpp"
#include "Resource/Texture.hpp"
#include "Shape/GL/Grid3D.hpp"
#include "Utils/Asset.hpp"

using mmm::vec2;
using mmm::vec3;

Terrain::Terrain() : Logging::Log("Terrain") {
  mGrid   = new GLGrid3D(vec2(128, 128));
  mShape  = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
  mMotion = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

  btRigidBody::btRigidBodyConstructionInfo consInfo(0,
                                                    mMotion,
                                                    mShape,
                                                    btVector3(0, 0, 0));
  mBody    = new btRigidBody(consInfo);
  mProgram = mAsset->rManager()->get<Program>("Program::Model");
  mTexture = mAsset->rManager()->get<Texture>("Texture::Terrain");
  mTexture->generateMipmaps();
  mTexture->linearMipmap();
  mScale = mmm::scale(150.0f, 0.0f, 150.0f);
}

Terrain::~Terrain() {
  delete mGrid;
  delete mBody;
  delete mShape;
  delete mMotion;
}

void Terrain::update(float) {}

void Terrain::drawShadow(Framebuffer* shadowMap, Camera* camera) {
  auto program = shadowMap->program();

  program->bind();
  program->setUniform("model", mmm::translate(mPosition) * mRotation * mScale);
  camera->setLightVPUniforms(program, "light");
  mGrid->draw();
}

void Terrain::draw(Camera* c) {
  mProgram->bind();
  mProgram->setUniform("model", mmm::translate(mPosition) * mRotation * mScale);
  mProgram->setUniform("view", c->view());
  mProgram->setUniform("proj", c->projection());

  mProgram->setUniform("dir", c->light().direction);
  c->setLightVPUniforms(mProgram, "light");

  mTexture->bind(1);
  mGrid->draw();
}

void Terrain::input(const Input::Event&) {}
