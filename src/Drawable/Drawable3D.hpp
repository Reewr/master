#pragma once

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;

class Asset;
class Camera;

#include "Drawable.hpp"
#include <mmm.hpp>

namespace Input {
  class Event;
}

class Drawable3D : public Drawable {
public:
  virtual ~Drawable3D();

  // Update the element, physics etc
  virtual void update(float deltaTime) = 0;

  // Draw it. Keep it separate from update
  virtual void draw(Camera* camera, float deltaTime) = 0;

  virtual void input(const Input::Event& event) = 0;

  void updateFromPhysics();

  bool hasPhysics();

  const mmm::vec3& position();

  btRigidBody* rigidBody();

protected:
  Drawable3D();

  mmm::vec3             mPosition;
  mmm::mat4             mScale;
  mmm::mat4             mRotation;
  btCollisionShape*     mShape;
  btDefaultMotionState* mMotion;
  btRigidBody*          mBody;
};
