#pragma once

#include "Drawable.hpp"

#include <memory>
#include <mmm.hpp>
#include <vector>

class btRigidBody;
class btCollisionShape;
class btMotionState;
class btTypedConstraint;

class Asset;
class Camera;
class Framebuffer;
class Program;

namespace Input {
  class Event;
}

class Drawable3D : public Drawable {
public:
  virtual ~Drawable3D();

  // Update the element, physics etc
  virtual void update(float deltaTime) = 0;

  // Draw it. Keep it separate from update
  virtual void draw(std::shared_ptr<Program>& program,
                    bool                      bindTexture = false) = 0;

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

  // Returns the rotation of the object
  const mmm::mat4& rotation() const;

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

  // Retrieves the torque from bullet
  mmm::vec3 torque() const;

  // Applies the torque to an object
  void setTorque(const mmm::vec3&);
  void setTorque(float x, float y, float z);

  // Retrieves the angular velocity from bullet
  mmm::vec3 angularVelocity() const;

  // Sets the angular velocity of an object
  void setAngularVelocity(const mmm::vec3&);
  void setAngularVelocity(float x, float y, float z);

  // Returns a reference to all children
  const std::vector<Drawable3D*>& children() const;

  // Adds a constraint to the object
  void addConstraint(btTypedConstraint* constraint);

  // Returns a reference to all constraints
  const std::vector<btTypedConstraint*> constraints() const;

  // Sets and gets different collision information
  int  collisionGroup() const;
  int  collisionMask() const;
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
