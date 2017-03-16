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
  virtual void drawShadow(Framebuffer* shadowMap, Camera* camera) = 0;

  // Draw it. Keep it separate from update
  virtual void draw(Camera* camera) = 0;

  // Handles input for the drawable object
  virtual void input(const Input::Event& event) = 0;

  // Whenever the world increments the physics, this method is called
  // to update the position and rotation of the object
  void updateFromPhysics();

  // If the element has shape, rigidbody and motion it is considered
  // to have physics
  bool hasPhysics() const;

  // Returns the position of the object
  const mmm::vec3& position() const;

  // Moves the object to a coordinate position
  void moveTo(float x, float y, float z);
  void moveTo(const mmm::vec3& position);

  // Rotates the object
  void rotate(float axisX, float axisY, float axisZ, float angle);
  void rotate(const mmm::vec3& axis, float angle);

  // Returns the rigidBody associated with the object
  btRigidBody* rigidBody();

  // Returns the objects weight in total, including its children
  float weight() const;

  mmm::vec3 torque() const;
  void setTorque(const mmm::vec3&);

  // Returns a reference to all children
  const std::vector<Drawable3D*>& children() const;

  // Adds a constraint to the object
  void addConstraint(btTypedConstraint* constraint);

  // Returns a reference to all constraints
  const std::vector<btTypedConstraint*> constraints() const;

  // Sets and gets different collision information
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
