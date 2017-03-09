#include "MeshPart.hpp"

#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"
#include "../GLSL/Program.hpp"

#include <btBulletDynamicsCommon.h>

MeshPart::MeshPart(std::shared_ptr<Program>& program,
                   const SubMeshPhysics& subMesh)
    : Logging::Log("MeshPart"), mMesh(subMesh.subMesh) {
  if (subMesh.body == nullptr)
    throw std::runtime_error("Rigid body was nullptr");

  if (subMesh.subMesh == nullptr)
    throw std::runtime_error("Submesh was nullptr");

  mBody    = subMesh.body;
  mShape   = mBody->getCollisionShape();

  const mmm::mat4& t = subMesh.subMesh->transform();
  // mmm::mat3 v = mmm::mat3(t[0].xyz, t[1].xyz, t[2].xyz);
  btMatrix3x3 mat;
  mat.setFromOpenGLSubMatrix(t.rawdata);
  mmm::vec3 p = mmm::dropRows<3>(t).xyz;
  btVector3 pos = btVector3(p.x, p.y, p.z);

  mMotion = new btDefaultMotionState(btTransform(mat, pos));
  mBody->setMotionState(mMotion);

  mProgram = program;
}

MeshPart::~MeshPart() {
  mBody->setMotionState(nullptr);
  delete mMotion;
}

void MeshPart::update(float) {}
void MeshPart::drawShadow(Framebuffer*, Camera*) {}

/**
 * @brief
 *   Draws the mesh part. Since the MeshPart is suppose to be part of a larger
 *   mesh, it expects a couple of things. Firstly, it expects most of the
 *   program variables to be set. It will only set the model variable.
 *
 * @param c
 */
void MeshPart::draw(Camera* c) {
  if (mMesh->size() == 0)
    return;

  mProgram->bind();
  mProgram->setUniform("model", mmm::translate(mPosition) * mRotation * mScale);

  mMesh->draw();
}

void MeshPart::input(const Input::Event&) {}
