#include "Cube.hpp"

#include <btBulletDynamicsCommon.h>

#include "../GLSL/Program.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Cube.hpp"
#include "../Utils/Asset.hpp"

using mmm::vec2;
using mmm::vec3;

Cube::Cube() {
  mCube    = new GLCube(vec2(256, 256));
  mProgram = mAsset->rManager()->get<Program>("Program::Model");
  mShape   = new btBoxShape(btVector3(1, 1, 1));
  mMotion  = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));

  btScalar mass = 1;
  btVector3 fallInertia(0, 0, 0);
  mShape->calculateLocalInertia(mass, fallInertia);
  btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
                                                    mMotion,
                                                    mShape,
                                                    fallInertia);
  mBody = new btRigidBody(consInfo);
  mPosition = vec3(0, 50, 0);
}

Cube::~Cube() {
  delete mCube;
  delete mBody;
  delete mShape;
  delete mMotion;
}

void Cube::update(float) {}

void Cube::draw(float) {
  mProgram->bind();
  mCube->draw();
}

void Cube::input(const Input::Event&) {}

bool Cube::hasPhysics() {
  return true;
}

btRigidBody* Cube::getRigidBody() {
  return mBody;
}

void Cube::updateFromPhysics() {
  btTransform trans;
  mBody->getMotionState()->getWorldTransform(trans);

  btVector3 origin = trans.getOrigin();
  mPosition = vec3(origin.x(), origin.y(), origin.z());
}
