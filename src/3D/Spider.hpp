#pragma once

#include <map>
#include <string>

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

class PhysicsMesh;
struct PhysicsElements;
class Program;

class btGeneric6DofSpringConstraint;
class btHingeConstraint;
class btTransform;

class Spider : public Drawable3D, public Logging::Log {
public:
  struct Part {
    Part();
    Part(unsigned short group, unsigned short mask, float angle, bool active);
    unsigned short                 collisionGroup;
    unsigned short                 collisionMask;
    float                          restAngle;
    bool                           active;
    Drawable3D*                    part;
    btHingeConstraint*             hinge;
    btGeneric6DofSpringConstraint* dof;
  };

  Spider();
  ~Spider();

  // Resets the positions, rotations and such for the whole spider
  void reset();

  void update(float deltaTime);

  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  void draw(std::shared_ptr<Program>& program,
            mmm::vec3                 offset,
            bool                      bindTexture = false);

  void input(const Input::Event& event);

  // Returns the child of spider if found by name
  Drawable3D* child(const std::string& name);

  std::map<std::string, Part>& parts();

  // Upcasts a Drawable3D objet to a Spider object, if possible.
  static Spider* upcast(Drawable3D* drawable);

  static std::map<std::string, Part> SPIDER_PARTS;

private:
  static std::map<std::string, btTransform> SPIDER_POSITIONS;

  PhysicsElements*             mElements;
  std::shared_ptr<PhysicsMesh> mMesh;
  std::map<std::string, Part> mParts;
};
