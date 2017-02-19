#pragma once

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

class Mesh;

class Spider : public Drawable3D, public Logging::Log {
public:
  Spider();
  ~Spider();

  void update(float deltaTime);

  void drawShadow(Framebuffer* shadowMap, Camera* camera);

  void draw(Camera* camera);

  void input(const Input::Event& event);

private:
  std::vector<std::shared_ptr<Mesh>> mMeshes;
};
