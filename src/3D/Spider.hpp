#pragma once

#include <map>
#include <string>

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

class Asset;
class PhysicsMesh;
struct PhysicsElements;
class Program;

class btHingeConstraint;

class Spider : public Drawable3D, public Logging::Log {
public:

  struct Part {
    Part();
    Part(unsigned short group, unsigned short mask);
    Part(unsigned short group, unsigned short mask, float torque);
    unsigned short collisionGroup;
    unsigned short collisionMask;

    btHingeConstraint* joint;
    float torque;
  };

  Spider(Asset*);
  ~Spider();

  void update(float deltaTime);

  void drawShadow(Framebuffer* shadowMap, Camera* camera);

  void draw(Camera* camera);

  void input(const Input::Event& event);

private:
  static std::map<std::string, Part> SPIDER_PARTS;

  std::shared_ptr<PhysicsMesh> mMesh;
  PhysicsElements*             mElements;
  std::shared_ptr<Program>     mProgram;
  std::map<std::string, Part>  mParts;
};
