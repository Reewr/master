#include "PhysicsMesh.hpp"
#include "Mesh.hpp"
#include "ResourceManager.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletWorldImporter.h>

PhysicsMesh::PhysicsMesh()
    : Logging::Log("PhysicsMesh"), mFileloader(nullptr), mMesh(nullptr) {}
PhysicsMesh::~PhysicsMesh() {}

/**
 * @brief
 *   Loads the physics from a bullet file and expects there to be an
 *   associated mesh that goes by the same name that this is requested
 *   as from the resource manager.
 *
 *   For instance, if a resource is called PhysicsMesh::Spider, it expects
 *   the matches the Mesh::Spider.
 *
 * @param manager
 *
 * @return
 */
bool PhysicsMesh::load(ResourceManager* manager) {
  if (loaded())
    return true;

  // Load the file with nullptr world as it does not require
  // that the world is specified. The objects will be added to
  // the world later
  mFileloader = new btBulletWorldImporter(nullptr);
  mFileloader->loadFile(filename().c_str());

  // Go through each rigid body, checking that their name is there
  // and that it is unique, logging warnings if not.
  for (int i = 0; i < mFileloader->getNumRigidBodies(); ++i) {
    btCollisionObject* obj  = mFileloader->getRigidBodyByIndex(i);
    btRigidBody*       body = btRigidBody::upcast(obj);

    const char* name = mFileloader->getNameForPointer(body);

    if (name == NULL) {
      mLog->warn("Rigid body without name!");
    }

    std::string strName = std::string(name);

    if (mBodies.count(strName))
      mLog->warn("Duplicate name '{}' for rigid body", strName);
    else
      mBodies[strName] = body;
  }

  // time to load the mesh for the physics
  std::string meshName = mName.substr(13);
  mMesh                = manager->get<Mesh>("Mesh::" + meshName);
}

/**
 * @brief
 *  Clears the bodies and deletes all rigid bodies that are stored
 *  within the fileloader.
 *
 *  Note: If there are any of these rigid bodies that have not been removed from
 *  the world, then this will cause the application to crash as Bullet will try
 *  to use these when they're nullptrs
 */
void PhysicsMesh::unload() {
  if (!loaded())
    return;

  mFileloader->deleteAllData();
  mBodies.clear();
}

/**
 * @brief
 *   Returns all the names of the rigid bodies
 *
 * @return
 */
std::vector<std::string> PhysicsMesh::names() {
  std::vector<std::string> names;
  names.reserve(mBodies.size());

  for (auto p : mBodies)
    names.push_back(p.first);

  return names;
}

/**
 * @brief
 *   Returns a structure that contains both the rigid body and the submesh that
 *   matches the name.
 *
 *   The pointers within this structure may be nulllptrs, if they are not found.
 *
 * @param name
 *
 * @return
 */
SubMeshPhysics PhysicsMesh::findByName(const std::string& name) {
  SubMeshPhysics s = { nullptr, nullptr };

  if (mBodies.count(name))
    s.body = mBodies[name];

  size_t index = mMesh->findMeshByName(name);

  if (index == Mesh::npos)
    return s;

  s.subMesh = &mMesh->getMeshByIndex(index);

  return s;
}

/**
 * @brief
 *   Checks if a string is within the vector
 *
 * @param v
 * @param s
 *
 * @return
 */
bool hasString(std::vector<std::string>& v, const std::string& s) {
  for (auto& element : v)
    if (element == s)
      return true;
  return false;
}

/**
 * @brief
 *   Retrieves all the names from the PhysicsMesh and the Mesh and merges these
 *   two lists together.
 *
 *   It then goes through this list and retrieves all available SubMeshPhysics
 *   where some of these may have nullptr SubMesh or nullptr.
 *
 * @return
 */
std::vector<std::pair<std::string, SubMeshPhysics>> PhysicsMesh::getAll() {
  std::vector<std::pair<std::string, SubMeshPhysics>> meshes;
  std::vector<std::string> meshNames = mMesh->names();

  for (auto& name : names()) {
    if (!hasString(meshNames, name))
      meshNames.push_back(name);
  }

  for (auto& name : meshNames) {
    meshes.push_back(std::make_pair(name, findByName(name)));
  }

  return meshes;
}
