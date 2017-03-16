#pragma once

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"
#include <memory>

class GLGrid3D;

class Texture;
class Program;

class Terrain : public Drawable3D, public Logging::Log {
public:
  Terrain();
  ~Terrain();

  // Update the element, physics etc
  void update(float deltaTime);

  // Draw it. Keep it separate from update
  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  // Input handler
  void input(const Input::Event& event);

private:
  GLGrid3D*                mGrid;
  std::shared_ptr<Texture> mTexture;
  std::shared_ptr<Program> mProgram;
};
