#pragma once

#include "Drawable.hpp"
#include <memory>

#include <mmm.hpp>

class GLCube;

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;

class Texture;
class Program;

class Cube : Drawable {
public:
  Cube();
  ~Cube();

  // Update the element, phusics etc
  void update(float deltaTime);

  // Draw it. Keep it seperate from update
  void draw(float deltaTime);

  void input(const Input::Event& event);

  bool hasPhysics();

  btRigidBody* getRigidBody();

  void updateFromPhysics();

private:
  GLCube*                  mCube;
  btRigidBody*             mBody;
  std::shared_ptr<Program> mProgram;

  mmm::vec3 mPosition;

  btCollisionShape*     mShape;
  btDefaultMotionState* mMotion;
};
