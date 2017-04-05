#pragma once

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"
#include <memory>

#include <mmm.hpp>

class GLSphere;
class Texture;
class Program;

/**
 * @brief
 *   This represents a Sphere in the world.
 **/
class Sphere : public Drawable3D, public Logging::Log {
public:
  // Creates a sphere of a specific size in meters.
  // The position is where the box will start at
  Sphere(const mmm::vec3& position,
         mmm::vec3 size = mmm::vec3(1.0),
         std::shared_ptr<Texture> texture = nullptr);
  ~Sphere();

  // Update the element, physics etc
  void update(float deltaTime);

  // Draw it. Keep it separate from update
  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  void draw(std::shared_ptr<Program>& program,
            mmm::vec3                 offset,
            bool                      bindTexture = false);

  // Input handler for sphere
  void input(const Input::Event& event);

private:
  GLSphere*                mSphere;
  std::shared_ptr<Texture> mTexture;
};
