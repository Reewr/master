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

    unsigned short collisionGroup;
    unsigned short collisionMask;

    btHingeConstraint* joint;
    mmm::vec3 torque;
  };

  Spider(Asset*);
  ~Spider();

  void update(float deltaTime);

  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  void input(const Input::Event& event);

private:
  static std::map<std::string, Part> SPIDER_PARTS;

  PhysicsElements*             mElements;
  std::shared_ptr<Program>     mProgram;
  std::shared_ptr<PhysicsMesh> mMesh;
  std::map<std::string, Part>  mParts;
};
