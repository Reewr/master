#pragma once

#include <memory>
#include "Drawable.hpp"

class GLGrid3D;

class btRigidBody;
class btCollisionShape;
class btDefaultMotionState;

class Texture;
class Program;

class Terrain : Drawable {
public:
  Terrain();
  ~Terrain();

  // Update the element, phusics etc
  void update(float deltaTime);

  // Draw it. Keep it seperate from update
  void draw(float deltaTime);

  void input(const Input::Event& event);

  bool hasPhysics();

  btRigidBody* getRigidBody();

  void updateFromPhysics();

private:
  GLGrid3D* mGrid;
  btRigidBody* mBody;
  std::shared_ptr<Texture> mTexture;
  std::shared_ptr<Program> mProgram;

  btCollisionShape* mShape;
  btDefaultMotionState* mMotion;
};
