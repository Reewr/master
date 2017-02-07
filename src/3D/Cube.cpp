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

Cube::Cube(const mmm::vec3& size, int weight, const mmm::vec3& position) {
  btVector3 btSize = btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
  mCube            = new GLCube();
  mShape           = new btBoxShape(btSize);
  mMotion          = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1),
                btVector3(position.x, position.y, position.z)));

  // Figure out the mass if not set
  // by assuming the cube contains water
  btScalar mass;
  if (weight == -1)
    mass = size.x * size.y * size.z * 1000;
  else
    mass = weight;

  btVector3 fallInertia(0, 0, 0);
  mShape->calculateLocalInertia(mass, fallInertia);
  btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
                                                    mMotion,
                                                    mShape,
                                                    fallInertia);
  mBody    = new btRigidBody(consInfo);
  mTexture = mAsset->rManager()->get<Texture>("Texture::Cube");
  mProgram = mAsset->rManager()->get<Program>("Program::Model");
  mScale   = mmm::scale(size);
}

Cube::~Cube() {
  delete mCube;
  delete mBody;
  delete mShape;
  delete mMotion;
}

/**
 * @brief
 *   The update handler is reversed for things that do not have
 *   anything to do with drawing but should be updated every loop.
 *
 * @param float
 */
void Cube::update(float) {}

/**
 * @brief
 *   Draw the cube by binding the program and setting the required
 *   uniforms.
 *
 * @param c
 * @param float
 */
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
