#include "Terrain.hpp"

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../GLSL/Program.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Grid3D.hpp"
#include "../Utils/Asset.hpp"
#include "Camera.hpp"

using mmm::vec2;

Terrain::Terrain() {
  mGrid    = new GLGrid3D(vec2(256, 256));
  mShape   = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
  mMotion  = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

  btRigidBody::btRigidBodyConstructionInfo consInfo(0,
                                                    mMotion,
                                                    mShape,
                                                    btVector3(0, 0, 0));
  mBody    = new btRigidBody(consInfo);
  mProgram = mAsset->rManager()->get<Program>("Program::Model");
  mTexture = mAsset->rManager()->get<Texture>("Texture::Terrain");
}

Terrain::~Terrain() {
  delete mGrid;
  delete mBody;
  delete mShape;
  delete mMotion;
}

void Terrain::update(float) {}

void Terrain::draw(Camera* c, float) {
  mProgram->bind();
  c->setMVPUniforms(mProgram);
  c->setLightVPUniforms(mProgram, "light");
  mTexture->bind(1);
  mGrid->draw();
}

void Terrain::input(const Input::Event&) {}
