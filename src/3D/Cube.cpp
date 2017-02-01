#include "Cube.hpp"

#include <btBulletDynamicsCommon.h>

#include "../GLSL/Program.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Cube.hpp"
#include "../Utils/Asset.hpp"
#include "Camera.hpp"

using mmm::vec2;
using mmm::vec3;

Cube::Cube() {
  mCube    = new GLCube(vec2(256, 256));
  mShape   = new btBoxShape(btVector3(0.5, 0.5, 0.5));
  mMotion  = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 20, 0)));

  btScalar  mass = 1000;
  btVector3 fallInertia(0, 0, 0);
  mShape->calculateLocalInertia(mass, fallInertia);
  btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
                                                    mMotion,
                                                    mShape,
                                                    fallInertia);
  mBody     = new btRigidBody(consInfo);
  mTexture  = mAsset->rManager()->get<Texture>("Texture::Cube");
  mProgram  = mAsset->rManager()->get<Program>("Program::Model");
  mScale    = mmm::scale(1.0f, 1.0f, 1.0f);
}

Cube::~Cube() {
  delete mCube;
  delete mBody;
  delete mShape;
  delete mMotion;
}

void Cube::update(float) {}

void Cube::draw(Camera* c, float) {
  mProgram->bind();
  mProgram->setUniform("model", mScale * mRotation * mmm::translate(mPosition));
  mProgram->setUniform("view", c->view());
  mProgram->setUniform("proj", c->projection());
  c->setLightVPUniforms(mProgram, "light");

  mTexture->bind(0);
  mCube->draw();
}

void Cube::input(const Input::Event&) {}
