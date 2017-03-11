#pragma once

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"

struct SubMeshPhysics;
class SubMesh;
class Program;

class MeshPart : public Drawable3D, public Logging::Log {
public:
  MeshPart(std::shared_ptr<Program>& program,
           const SubMesh*            subMesh,
           btRigidBody*              body,
           btMotionState*            motion);
  ~MeshPart();

  // Update the element, physics etc
  void update(float deltaTime);

  // First pass draw from light's point of view
  void drawShadow(Framebuffer* shadowMap, Camera* camera);

  // Draw it. Keep it separate from update
  void draw(Camera* camera);

  void input(const Input::Event& event);

private:
  const SubMesh*           mMesh;
  std::shared_ptr<Program> mProgram;
};
