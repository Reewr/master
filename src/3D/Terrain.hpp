#pragma once

#include "../Drawable/Drawable3D.hpp"
#include <memory>

class GLGrid3D;

class Texture;
class Program;

class Terrain : public Drawable3D {
public:
  Terrain();
  ~Terrain();

  // Update the element, phusics etc
  void update(float deltaTime);

  // Draw it. Keep it seperate from update
  void draw(Camera* camera);

  // Input handler
  void input(const Input::Event& event);

private:
  GLGrid3D*                mGrid;
  std::shared_ptr<Texture> mTexture;
  std::shared_ptr<Program> mProgram;
};
