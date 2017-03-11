#pragma once

#include <map>
#include <string>

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

class Asset;
class PhysicsMesh;
class PhysicsElements;
class Program;

enum SpiderPart {
  Abdomen,
  AbdomenInner,
  Coxa,
  Femur,
  FemurTibiaInner,
  Head,
  HeadDisk,
  HeadInner,
  Hip,
  Joint,
  JointRot,
  Metatarsus,
  Neck,
  Patella,
  PatellaInner,
  SternumBack,
  SternumFront,
  TarsusBot,
  TarsusTop,
  Thorax,
  Tibia,
  Trochanter
};

std::string toString(SpiderPart);

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
