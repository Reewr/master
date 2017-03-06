#include "MeshPart.hpp"

#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"

#include <btBulletDynamicsCommon.h>

MeshPart::MeshPart(const SubMeshPhysics& subMesh)
    : Logging::Log("MeshPart"), mMesh(subMesh.subMesh) {
  if (subMesh.body == nullptr)
    throw std::runtime_error("Rigid body was nullptr");

  if (subMesh.subMesh == nullptr)
    throw std::runtime_error("Submesh was nullptr");

  mBody   = subMesh.body;
  mShape  = mBody->getCollisionShape();
  mMotion = mBody->getMotionState();
}

MeshPart::~MeshPart() {}

void MeshPart::update(float) {}
void MeshPart::drawShadow(Framebuffer*, Camera*) {}
void MeshPart::draw(Camera*) {}
void MeshPart::input(const Input::Event&) {}
