#pragma once

#include "Resource.hpp"
#include "Mesh.hpp"
#include "Log.hpp"

#include <memory>
#include <map>
#include <vector>
#include <string>

class ResourceManager;

class btRigidBody;
class btBulletWorldImporter;

class PhysicsMesh : public Resource, public Logging::Log {
public:

  struct SubMeshPhysics {
    const Mesh::SubMesh* subMesh;
    btRigidBody* body;
  };

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
