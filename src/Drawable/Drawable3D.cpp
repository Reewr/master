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
    , mCollisionMask(btBroadphaseProxy::AllFilter ^
                     btBroadphaseProxy::StaticFilter) {}

Drawable3D::~Drawable3D() {}

/**
 * @brief
 *   When the element has physics and has been added to the world, the world
 *   will make sure to keep the elements updated.
 *
 *   In order to draw the elements correctly, the world will keep calling this
 *   function whenever physics steps has been performed to update the new
 *   rotation and position
 */
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

/**
 * @brief
 *   Returns the stored position that is often updated by
 *   bullet.
 *
 * @return
 */
const mmm::vec3& Drawable3D::position() const {
  return mPosition;
}

/**
 * @brief
 *   Sets the new position of the object. The position is based on
 *   the mass center of the object.
 *
 *   When the position is set, the forces are cleared from the object
 *   and it is activated again to start performing physics.
 *
 * @param x
 * @param y
 * @param z
 */
void Drawable3D::moveTo(float x, float y, float z) {
  const btVector3 pos = btVector3(x, y, z);

  if (hasPhysics()) {
    btTransform trans = mBody->getCenterOfMassTransform();
    trans.setOrigin(pos);

    mBody->setCenterOfMassTransform(trans);
    mBody->clearForces();
    mBody->activate();
  }

  mPosition = mmm::vec3(x, y, z);

  for (auto& c : mChildren) {
    const mmm::vec3& childPos = c->position();
    c->moveTo(x - childPos.x, y - childPos.y, z - childPos.z);
  }
}

/**
 * @brief
 *   Sets the new position of the object. The position is based on
 *   the mass center of the object.
 *
 *   When the position is set, the forces are cleared from the object
 *   and it is activated again to start performing physics.
 *
 * @param position
 */
void Drawable3D::moveTo(const mmm::vec3& position) {
  moveTo(position.x, position.y, position.z);
}

/**
 * @brief
 *   Rotates the object around a specific axis. By specifying a number in the
 *   axis variables, you tell bullet to rotate the object around that axis.
 *
 *   The angle is in degrees.
 *
 * @param axisX
 * @param axisY
 * @param axisZ
 * @param angle
 */
void Drawable3D::rotate(float axisX, float axisY, float axisZ, float angle) {
  const btVector3 axisBt = btVector3(axisX, axisY, axisZ);

  if (hasPhysics()) {
    btTransform trans = mBody->getCenterOfMassTransform();
    trans.setRotation(btQuaternion(axisBt, mmm::radians(angle)));

    mBody->setCenterOfMassTransform(trans);
    mBody->clearForces();
    mBody->activate();
  }

  for (auto& c : mChildren) {
    c->rotate(axisX, axisY, axisZ, angle);
  }
}

/**
 * @brief
 *   Rotates the object around a specific axis. By specifying a number in the
 *   axis variables, you tell bullet to rotate the object around that axis.
 *
 *   The angle is in degrees.
 *
 * @param axis
 * @param angle
 */
void Drawable3D::rotate(const mmm::vec3& axis, float angle) {
  rotate(axis.x, axis.y, axis.z, angle);
}

/**
 * @brief
 *   If the object has a stored shape, motion and body it is considered
 *   to have physics.
 *
 * @return
 */
bool Drawable3D::hasPhysics() const {
  return mShape != nullptr && mMotion != nullptr && mBody != nullptr;
}

/**
 * @brief
 *   Returns the pointer to the rigid body used with the object.
 *
 *   Keep in mind that this may be a nullptr.
 *
 * @return
 */
btRigidBody* Drawable3D::rigidBody() {
  return mBody;
}

/**
 * @brief
 *   Returns the weight of the object and all its children
 *   summed together.
 *
 * @return
 */
float Drawable3D::weight() const {
  float w = 0;

  if (hasPhysics())
    w += mBody->getInvMass();

  for (auto& child : mChildren) {
    w += child->weight();
  }

  return w;
}

/**
 * @brief
 *   Returns the torque saved in bullet for the specific
 *   object. Does not take child objects into consideration
 *
 * @return
 */
mmm::vec3 Drawable3D::torque() const {
  if (!hasPhysics())
    return mmm::vec3(0);

  const btVector3& torque = mBody->getTotalTorque();
  return mmm::vec3(torque.x(), torque.y(), torque.z());
}

/**
 * @brief
 *   Sets the torque of an object. Does not apply torque to child elements.
 *
 * @param torque
 */
void Drawable3D::setTorque(const mmm::vec3& torque) {
  setTorque(torque.x, torque.y, torque.z);
}

/**
 * @brief
 *   Sets the torque of an object. Does not apply torque to child elements.
 *
 * @param x
 * @param y
 * @param z
 */
void Drawable3D::setTorque(float x, float y, float z) {
  if (!hasPhysics())
    return;

  mBody->applyTorque(btVector3(x, y, z));
}

/**
 * @brief
 *   Returns a reference to all the children of the object
 *
 * @return
 */
const std::vector<Drawable3D*>& Drawable3D::children() const {
  return mChildren;
}

/**
 * @brief
 *   Adds a constraint to the object. This should be used whenever
 *   the object has a constraint attached to it directly or through
 *   a seperate object.
 *
 * @param constraint
 */
void Drawable3D::addConstraint(btTypedConstraint* constraint) {
  mConstraints.push_back(constraint);
}

/**
 * @brief
 *   Returns a reference to all saved constraints
 *
 * @return
 */
const std::vector<btTypedConstraint*> Drawable3D::constraints() const {
  return mConstraints;
}

/**
 * @brief
 *   Returns the stored collision group
 *
 * @return
 */
int Drawable3D::collisionGroup() const {
  return mCollisionGroup;
}

/**
 * @brief
 *   Returns the stored collision mask
 *
 * @return
 */
int Drawable3D::collisionMask() const {
  return mCollisionMask;
}

/**
 * @brief
 *   sets the stored collision group
 *
 * @return
 */
void Drawable3D::setCollisionGroup(int x) {
  mCollisionGroup = x;
}

/**
 * @brief
 *   sets the stored collision mask
 *
 * @return
 */
void Drawable3D::setCollisionMask(int x) {
  mCollisionMask = x;
}
