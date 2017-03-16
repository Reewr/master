#pragma once

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"
#include <memory>

#include <mmm.hpp>

class GLCube;

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;

class Texture;
class Program;

/**
 * @brief
 *   This represents a Cube with Physics in the world. It can be changed
 *   to have a certain size and weight and can also be specified to be
 *   at a specified position
 */
class Cube : public Drawable3D, public Logging::Log {
public:
  // Creates a cube of a specific size in meters. If weight is -1,
  // it is assumed that the cube contains water,
  // i.e 1m^3 (1,1,1) cube weighs 1000kg
  //     8m^3 (2,2,2) cube weighs 8000kg
  //
  // The position is where the box will start at
  Cube(const mmm::vec3& size     = mmm::vec3(1.0f, 1.0f, 1.0f),
       int              weight   = -1,
       const mmm::vec3& position = mmm::vec3(0, 20, 0));
  ~Cube();

  // Update the element, physics etc
  void update(float deltaTime);

  // Draw it. Keep it separate from update
  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  // Input handler for cube
  void input(const Input::Event& event);

private:
  GLCube*                  mCube;
  std::shared_ptr<Program> mProgram;
  std::shared_ptr<Texture> mTexture;
};
