#include "Drawable3D.hpp"

#include <btBulletDynamicsCommon.h>

using mmm::vec3;

Drawable3D::Drawable3D()
    : mPosition(0, 0, 0)
    , mScale(mmm::mat4::identity)
    , mRotation(mmm::mat4::identity)
    , mShape(nullptr)
    , mMotion(nullptr)
    , mBody(nullptr) {}

Drawable3D::~Drawable3D() {}

void Drawable3D::updateFromPhysics() {
  btTransform trans;
  mBody->getMotionState()->getWorldTransform(trans);

  btVector3 origin      = trans.getOrigin();
  mPosition             = vec3(origin.x(), origin.y(), origin.z());
  btQuaternion rotation = trans.getRotation().normalized();

  float qx  = rotation.getX();
  float qy  = rotation.getY();
  float qz  = rotation.getZ();
  float qw  = rotation.getW();
  mRotation = mmm::mat4(1.0f - 2.0f * qy * qy - 2.0f * qz * qz,
                        2.0f * qx * qy - 2.0f * qz * qw,
                        2.0f * qx * qz + 2.0f * qy * qw,
                        0.0f,
                        2.0f * qx * qy + 2.0f * qz * qw,
                        1.0f - 2.0f * qx * qx - 2.0f * qz * qz,
                        2.0f * qy * qz - 2.0f * qx * qw,
                        0.0f,
                        2.0f * qx * qz - 2.0f * qy * qw,
                        2.0f * qy * qz + 2.0f * qx * qw,
                        1.0f - 2.0f * qx * qx - 2.0f * qy * qy,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        1.0f);
}

const mmm::vec3& Drawable3D::position() {
  return mPosition;
}

bool Drawable3D::hasPhysics() {
  return mShape != nullptr && mMotion != nullptr && mBody != nullptr;
}

btRigidBody* Drawable3D::rigidBody() {
  return mBody;
}
