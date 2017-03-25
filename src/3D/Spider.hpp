#pragma once

#include <map>
#include <string>

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

class PhysicsMesh;
struct PhysicsElements;
class Program;

class btHingeConstraint;
class btTransform;

class Spider : public Drawable3D, public Logging::Log {
public:
  struct Part {
    Part();
    Part(unsigned short   group,
         unsigned short   mask,
         const mmm::vec3& inputAngX,
         const mmm::vec3& inputAngY,
         const mmm::vec3& inputAngZ,
         const mmm::vec3& inputRot,
         const mmm::vec3& output);

    unsigned short collisionGroup;
    unsigned short collisionMask;

    mmm::vec3 inputAngX;
    mmm::vec3 inputAngY;
    mmm::vec3 inputAngZ;
    mmm::vec3 inputRot;
    mmm::vec3 output;

    Drawable3D* part;
  };

  Spider();
  ~Spider();

  // Resets the positions, rotations and such for the whole spider
  void reset();

  void update(float deltaTime);

  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  void input(const Input::Event& event);

  // Returns the child of spider if found by name
  Drawable3D* child(const std::string& name);

  // Upcasts a Drawable3D objet to a Spider object, if possible.
  static Spider* upcast(Drawable3D* drawable);

private:
  static std::map<std::string, Part>        SPIDER_PARTS;
  static std::map<std::string, btTransform> SPIDER_POSITIONS;

  PhysicsElements*             mElements;
  std::shared_ptr<PhysicsMesh> mMesh;
  std::map<std::string, Part> mParts;
};
