#include "MeshPart.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"

#include <btBulletDynamicsCommon.h>

mmm::vec3 tovec(const btVector3& m) {
  return mmm::vec3(m.x(), m.y(), m.z());
}

MeshPart::MeshPart(const SubMesh* subMesh,
                   btRigidBody*   body,
                   btMotionState* motion)
    : Logging::Log("MeshPart"), mMesh(subMesh) {
  mMotion = motion;
  mBody   = body;
  mShape  = mBody->getCollisionShape();

  // mLog->debug(
  //   "{}: Gravity: {}, Mass: {}, Shape: {}, LocalInertia: {}, Constraints:
  //   {}",
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

void MeshPart::draw(std::shared_ptr<Program>& program, bool bindTexture) {
  draw(program, mmm::vec3(0), bindTexture);
}

/**
 * @brief
 *   Draws the mesh part. Since the MeshPart is suppose to be part of a larger
 *   mesh, it expects a couple of things. Firstly, it expects most of the
 *   program variables to be set. It will only set the model variable.
 *
 * @param program
 * @param offset
 * @param bindTexture
 */
void MeshPart::draw(std::shared_ptr<Program>& program,
                    mmm::vec3                 offset,
                    bool                      bindTexture) {
  if (mMesh->size() == 0)
    return;

  program->bind();
  program->setUniform("model",
                      mmm::translate(mPosition + offset) * mRotation * mScale);

  mMesh->draw(bindTexture ? 1 : -1);
}

void MeshPart::input(const Input::Event&) {}
