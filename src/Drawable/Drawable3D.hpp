#pragma once

class btRigidBody;
class btCollisionShape;
class btMotionState;
class btTypedConstraint;

class Asset;
class Camera;
class Framebuffer;

#include "Drawable.hpp"

#include <mmm.hpp>
#include <vector>

namespace Input {
  class Event;
}

class Drawable3D : public Drawable {
public:
  virtual ~Drawable3D();

  // Update the element, physics etc
  virtual void update(float deltaTime) = 0;

  // First pass draw from light's point of view
  virtual void drawShadow(Framebuffer* shadowMap, Camera* camera);

  // Draw it. Keep it separate from update
  virtual void draw(Camera* camera) = 0;

  virtual void input(const Input::Event& event) = 0;

  void updateFromPhysics();

  bool hasPhysics();

  const mmm::vec3& position();

  void moveTo(float x, float y, float z);
  void moveTo(const mmm::vec3& position);

  void rotate(float axisX, float axisY, float axisZ, float angle);
  void rotate(const mmm::vec3& axis, float angle);

  btRigidBody* rigidBody();

  float weight();

  const std::vector<Drawable3D*>& children();

  void addConstraint(btTypedConstraint* constraint);

  const std::vector<btTypedConstraint*> constraints();

  int collisionGroup() const;
  int collisionMask() const;
  void setCollisionGroup(int);
  void setCollisionMask(int);

protected:
  Drawable3D();

  mmm::vec3                       mPosition;
  mmm::mat4                       mScale;
  mmm::mat4                       mRotation;
  btCollisionShape*               mShape;
  btMotionState*                  mMotion;
  btRigidBody*                    mBody;
  std::vector<btTypedConstraint*> mConstraints;
  std::vector<Drawable3D*>        mChildren;

  int mCollisionGroup;
  int mCollisionMask;
};
