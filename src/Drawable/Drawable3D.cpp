#include "Drawable3D.hpp"

#include "3D/Camera.hpp"
#include "Graphical/Framebuffer.hpp"

#include <btBulletDynamicsCommon.h>

using mmm::vec3;

Drawable3D::Drawable3D()
    : mPosition(0, 0, 0)
    , mScale(mmm::mat4::identity)
    , mRotation(mmm::mat4::identity)
    , mShape(nullptr)
    , mMotion(nullptr)
    , mBody(nullptr)
    , mChildren({})
    , mCollisionGroup(btBroadphaseProxy::StaticFilter)
    , mCollisionMask(btBroadphaseProxy::AllFilter^btBroadphaseProxy::StaticFilter) {}

Drawable3D::~Drawable3D() {}

void Drawable3D::drawShadow(Framebuffer*, Camera*) {}

void Drawable3D::updateFromPhysics() {
  if (hasPhysics()) {
    btTransform trans;
    mMotion->getWorldTransform(trans);

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

  for (auto& child : mChildren)
    child->updateFromPhysics();
}

const mmm::vec3& Drawable3D::position() {
  return mPosition;
}

void Drawable3D::moveTo(const mmm::vec3& position) {
  const btVector3 pos = btVector3(position.x, position.y, position.z);

  if (hasPhysics()) {
    btTransform trans = mBody->getCenterOfMassTransform();
    trans.setOrigin(pos);

    mBody->setCenterOfMassTransform(trans);
    mBody->clearForces();
  }

  mPosition = position;

  for (auto& c : mChildren) {
    c->moveTo(position - c->position());
  }
}

void Drawable3D::rotate(const mmm::vec3& axis, float angle) {
  const btVector3 axisBt = btVector3(axis.x, axis.y, axis.z);

  if (hasPhysics()) {
    btTransform trans = mBody->getCenterOfMassTransform();
    trans.setRotation(btQuaternion(axisBt, mmm::radians(angle)));

    mBody->setCenterOfMassTransform(trans);
    mBody->clearForces();
  }

  for (auto& c : mChildren) {
    c->rotate(axis, angle);
  }
}

bool Drawable3D::hasPhysics() {
  return mShape != nullptr && mMotion != nullptr && mBody != nullptr;
}

btRigidBody* Drawable3D::rigidBody() {
  return mBody;
}

float Drawable3D::weight() {
  float w = 0;

  if (hasPhysics())
    w += mBody->getInvMass();

  for (auto& child : mChildren) {
    w += child->weight();
  }

  return w;
}

const std::vector<Drawable3D*>& Drawable3D::children() {
  return mChildren;
}

void Drawable3D::addConstraint(btTypedConstraint* constraint) {
  mConstraints.push_back(constraint);
}

const std::vector<btTypedConstraint*> Drawable3D::constraints() {
  return mConstraints;
}

int Drawable3D::collisionGroup() const {
  return mCollisionGroup;
}
int Drawable3D::collisionMask() const {
  return mCollisionMask;
}

void Drawable3D::setCollisionGroup(int x) {
  mCollisionGroup = x;
}
void Drawable3D::setCollisionMask(int x) {
  mCollisionMask = x;
}
