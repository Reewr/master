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
class btMotionState;

/**
 * @brief
 *   A SubMesh Physics
 */
struct SubMeshPhysics {
  const SubMesh* subMesh;
  btRigidBody*   body;
  std::string    name;
  std::vector<btTypedConstraint*> constraints;
};

struct PhysicsElements {
  std::map<std::string, const SubMesh*> meshes;
  std::map<std::string, btRigidBody*> bodies;
  std::map<std::string, btMotionState*> motions;
  std::map<std::string, std::vector<btTypedConstraint*>> constraints;
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

  // Creates a copy of all the physics elements stored
  // and returns those to you.
  //
  // The reason for this function is because you cannot add two of the same
  // rigidbodies to a world. This copies each rigidBody, sets a motion state
  // based on the transforms in the Mesh and setup the constraints.
  PhysicsElements* createCopyAll();

  // Releases the resources allocated for a copy of the elements.
  //
  // When this function is called, it is expected that it is detached from
  // the world.
  void deleteCopy(PhysicsElements* copy);

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
  std::map<btRigidBody*, std::string> mNames;

  std::vector<PhysicsElements> mCopies;
};
