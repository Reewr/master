#pragma once

#include "Resource.hpp"
#include "Log.hpp"

#include <memory>
#include <map>
#include <vector>
#include <string>

class ResourceManager;
class SubMesh;
class Mesh;

class btRigidBody;
class btBulletWorldImporter;

struct SubMeshPhysics {
  const SubMesh* subMesh;
  btRigidBody* body;
};

class PhysicsMesh : public Resource, public Logging::Log {
public:

  PhysicsMesh();
  ~PhysicsMesh();

  // Loads the physics mesh from file
  bool load(ResourceManager* manager);

  // Unloads the Physics mesh
  void unload();

  // Returns the names for all rigid bodies
  std::vector<std::string> names();

  // Searches both physics bodies and mesh bodies to find some
  // that matches together by name.
  // Both pointers in this structure may be null
  SubMeshPhysics findByName(const std::string& name);

private:
  btBulletWorldImporter* mFileloader;
  std::shared_ptr<Mesh> mMesh;
  std::map<std::string, btRigidBody*> mBodies;
};
