#include "Drawable3D.hpp"

#include <btBulletDynamicsCommon.h>

using mmm::vec3;

Drawable3D::Drawable3D()
    : mPosition(0, 0, 0), mShape(nullptr), mMotion(nullptr), mBody(nullptr) {}

Drawable3D::~Drawable3D() {}

void Drawable3D::updateFromPhysics() {
  btTransform trans;
  mBody->getMotionState()->getWorldTransform(trans);

  btVector3 origin = trans.getOrigin();
  mPosition        = vec3(origin.x(), origin.y(), origin.z());
}

bool Drawable3D::hasPhysics() {
  return mShape != nullptr && mMotion != nullptr && mBody != nullptr;
}

btRigidBody* Drawable3D::getRigidBody() {
  return mBody;
}
