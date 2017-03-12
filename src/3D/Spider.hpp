#pragma once

#include <map>
#include <string>

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

class Asset;
class PhysicsMesh;
struct PhysicsElements;
class Program;


class Spider : public Drawable3D, public Logging::Log {
public:
  Spider(Asset*);
  ~Spider();

  void update(float deltaTime);

  void drawShadow(Framebuffer* shadowMap, Camera* camera);

  void draw(Camera* camera);

  void input(const Input::Event& event);

private:
  std::shared_ptr<PhysicsMesh> mMesh;
  PhysicsElements*             mElements;
  std::shared_ptr<Program>     mProgram;
};
