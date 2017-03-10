#pragma once

#include "Log.hpp"
#include "Resource.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

class ResourceManager;
class SubMesh;
class Mesh;

class btRigidBody;
class btBulletWorldImporter;
class btTypedConstraint;

/**
 * @brief
 *   A SubMesh Physics
 */
struct SubMeshPhysics {
  const SubMesh*     subMesh;
  const btRigidBody* body;
  std::string name;
  std::vector<const btTypedConstraint*> constraints;
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

  // Returns all the submeshes and rigid bodies merged together
  // for one mesh together with the name of the given mesh/rigid body.
  //
  // This may also include mesh without rigid body and rigid body without
  // mesh
  std::vector<std::pair<std::string, SubMeshPhysics>> getAll();

  // Returns the mesh
  const std::shared_ptr<Mesh>& mesh() const;

private:
  std::string findNameByPointer(btRigidBody* body);

  btBulletWorldImporter* mFileloader;
  std::shared_ptr<Mesh>  mMesh;

  std::map<std::string, std::vector<btTypedConstraint*>> mConstraints;
  std::map<std::string, btRigidBody*> mBodies;
};
