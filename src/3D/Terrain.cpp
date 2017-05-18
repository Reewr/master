#include "Terrain.hpp"

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../Camera/Camera.hpp"
#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Grid3D.hpp"
#include "../Utils/Asset.hpp"

using mmm::vec2;
using mmm::vec3;
using mmm::mat4;
using RigidBodyInfo = btRigidBody::btRigidBodyConstructionInfo;

Terrain::Terrain() : Logging::Log("Terrain") {
  mGrid   = new GLGrid3D(vec2(16, 16));
  mShape  = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
  mMotion = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

  RigidBodyInfo consInfo(0, mMotion, mShape, btVector3(0, 0, 0));
  consInfo.m_friction = 1.0;

  mBody    = new btRigidBody(consInfo);
  mTexture = mAsset->rManager()->get<Texture>("Texture::Terrain");
  mTexture->generateMipmaps();
  mTexture->linearMipmap();
  mTexture->repeat();
  /* mScale = mmm::scale(38.0f, 0.0f, 38.0f); */
  mScale = mmm::scale(64.0f, 0.0f, 64.0f);
}

Terrain::~Terrain() {
  delete mGrid;
  delete mBody;
  delete mShape;
  delete mMotion;
}

void Terrain::update(float) {}

void Terrain::draw(std::shared_ptr<Program>& program, bool bindTexture) {
  draw(program, mmm::vec3(0), bindTexture);
}

void Terrain::draw(std::shared_ptr<Program>& program,
                   mmm::vec3                 offset,
                   bool                      bindTexture) {
  mat4 model =
    mmm::translate(mPosition + vec3(0, 1, 0) + offset) * mRotation * mScale;

  program->bind();
  program->setUniform("model", model);

  if (bindTexture)
    mTexture->bind(1);
  mGrid->draw();
}

void Terrain::input(const Input::Event&) {}
