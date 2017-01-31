#pragma once

#include "../Drawable/Drawable3D.hpp"
#include <memory>

#include <mmm.hpp>

class GLCube;

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;

class Texture;
class Program;

class Cube : public Drawable3D {
public:
  Cube();
  ~Cube();

  // Update the element, phusics etc
  void update(float deltaTime);

  // Draw it. Keep it seperate from update
  void draw(Camera* camera, float deltaTime);

  void input(const Input::Event& event);

private:
  GLCube*                  mCube;
  std::shared_ptr<Program> mProgram;
  std::shared_ptr<Texture> mTexture;
};
