#pragma once

#include <map>
#include <string>

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

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
    Drawable3D*    part;
  };

  Spider();
  ~Spider();

  void update(float deltaTime);

  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  void input(const Input::Event& event);

  // Returns the child of spider if found by name
  Drawable3D* child(const std::string& name);

  // Upcasts a Drawable3D objet to a Spider object, if possible.
  static Spider* upcast(Drawable3D* drawable);

private:
  static std::map<std::string, Part> SPIDER_PARTS;

  PhysicsElements*             mElements;
  std::shared_ptr<PhysicsMesh> mMesh;
  std::map<std::string, Part> mParts;
};
