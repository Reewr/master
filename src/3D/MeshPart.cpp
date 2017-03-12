#include "MeshPart.hpp"

#include "../GLSL/Program.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"

#include <btBulletDynamicsCommon.h>

mmm::vec3 tovec(const btVector3& m) {
  return mmm::vec3(m.x(), m.y(), m.z());
}

MeshPart::MeshPart(std::shared_ptr<Program>& program,
                   const SubMesh*            subMesh,
                   btRigidBody*              body,
                   btMotionState*            motion)
    : Logging::Log("MeshPart"), mMesh(subMesh), mProgram(program) {
  mMotion = motion;
  mBody   = body;
  mShape  = mBody->getCollisionShape();

  // mLog->debug(
  //   "{}: Gravity: {}, Mass: {}, Shape: {}, LocalInertia: {}, Constraints: {}",
  //   subMesh->name(),
  //   tovec(mBody->getGravity()),
  //   mBody->getInvMass(),
  //   mShape->getName(),
  //   tovec(mBody->getLocalInertia()),
  //   mBody->getNumConstraintRefs());
  updateFromPhysics();
}

MeshPart::~MeshPart() {}

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
void MeshPart::draw(Camera*) {
  if (mMesh->size() == 0)
    return;

  mProgram->bind();
  mProgram->setUniform("model", mmm::translate(mPosition) * mRotation * mScale);

  mMesh->draw();
}

void MeshPart::input(const Input::Event&) {}
