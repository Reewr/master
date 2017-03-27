#include "PhysicsMesh.hpp"
#include "Mesh.hpp"
#include "ResourceManager.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletWorldImporter.h>

using RigidBodyInfo = btRigidBody::btRigidBodyConstructionInfo;

PhysicsMesh::PhysicsMesh()
    : Logging::Log("PhysicsMesh"), mFileloader(nullptr), mMesh(nullptr) {}

PhysicsMesh::~PhysicsMesh() {
  unload();
}

std::string PhysicsMesh::findNameByPointer(btRigidBody* body) {
  if (mNames.count(body))
    return mNames[body];
  return "";
}

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
    else {
      mLog->debug("Loaded rigid body: '{}'", strName);
      mBodies[strName] = body;
      mNames[body]     = strName;
    }
  }

  for (int i = 0; i < mFileloader->getNumConstraints(); ++i) {
    btTypedConstraint* c              = mFileloader->getConstraintByIndex(i);
    const std::string& fromConstraint = findNameByPointer(&c->getRigidBodyA());
    const std::string& toConstraint   = findNameByPointer(&c->getRigidBodyB());

    mConstraints[toConstraint].push_back(c);
    mConstraints[fromConstraint].push_back(c);

    mLog->debug("Constraint: {}, fromObject: {}, toObject: {}",
                i,
                fromConstraint,
                toConstraint);
  }

  // time to load the mesh for the physics
  size_t position = mName.find("::");

  if (position == std::string::npos) {
    mLog->error("Name does not contain '::', {}", mName);
    return false;
  }

  std::string meshName = mName.substr(position + 2);

  mLog->debug("Loading mesh :: {}", meshName);

  std::string fullName = "Mesh::" + meshName;
  mMesh                = manager->get<Mesh>(fullName);

  setLoaded(true);
  return true;
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

  for (auto& storedCopy : mCopies) {
    // Delete constraints first
    for (auto& el : storedCopy.constraints) {
      for (auto& innerEl : el.second) {
        if (storedCopy.bodies[el.first] == &innerEl->getRigidBodyA()) {
          delete innerEl;
          innerEl = nullptr;
        }
      }

      el.second.clear();
    }

    // Followed by all rigidBodies
    for (auto& el : storedCopy.bodies)
      delete el.second;

    for (auto& el : storedCopy.motions)
      delete el.second;


    // Now that all copies have been deleted, clear the maps
    // of strings.
    storedCopy.bodies.clear();
    storedCopy.motions.clear();
    storedCopy.constraints.clear();
  }

  for (auto& a : mConstraints)
    a.second.clear();

  mCopies.clear();
  mConstraints.clear();
  mBodies.clear();
  mNames.clear();
  mFileloader->deleteAllData();

  setLoaded(false);
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
  SubMeshPhysics s = { nullptr, nullptr, name, {} };

  if (mBodies.count(name))
    s.body = mBodies[name];

  if (mConstraints.count(name))
    s.constraints = mConstraints[name];

  size_t index = mMesh->findMeshByName(name);

  if (index == Mesh::npos)
    return s;

  s.subMesh = &mMesh->getMeshByIndex(index);

  return s;
}

/**
 * @brief
 *   Creates a copy of all the physics elements stored and returns those to you.
 *   The reason for this function is because you cannot add two of the same
 *   rigidbodies to a world. This copies each rigidBody, sets a motion state
 *   based on the transforms in the Mesh and setup the constraints.
 *
 * @return
 */
PhysicsElements* PhysicsMesh::createCopyAll() {
  auto            meshes = getAll();
  PhysicsElements elements;

  // First create copies of the rigid bodies together with new
  // motion state based on the internal variables of the rigid bodies
  for (auto& mesh : meshes) {
    btRigidBody*      mainBody = mesh.second.body;
    btCollisionShape* shape    = mainBody->getCollisionShape();

    btMatrix3x3      mat;
    const mmm::mat4& t      = mesh.second.subMesh->transform();
    const mmm::vec3& matPos = mmm::dropColumns<3>(t).xyz;

    // TODO fix static +2 up translation
    const btVector3  pos     = btVector3(matPos.x, matPos.y + 1, matPos.z);
    const btVector3& inertia = mainBody->getLocalInertia();
    const btScalar   mass    = mainBody->getInvMass();

    mat.setFromOpenGLSubMatrix(mmm::transpose(t).rawdata);

    btMotionState* motion = new btDefaultMotionState(btTransform(mat, pos));
    // btMotionState* motion = new
    // btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0,
    // 0, 0)));
    btRigidBody* body = new btRigidBody(mass, motion, shape, inertia);

    elements.bodies[mesh.first]  = body;
    elements.motions[mesh.first] = motion;
    elements.meshes[mesh.first]  = mesh.second.subMesh;
  }

  // Now that all bodies and motions have been created, the constraints can be
  // duplicated.
  //
  // Since different constraints may contain different variables, they have to
  // be handled seperately
  for (int i = 0; i < mFileloader->getNumConstraints(); ++i) {
    btTypedConstraint* c              = mFileloader->getConstraintByIndex(i);
    const std::string& fromConstraint = findNameByPointer(&c->getRigidBodyA());
    const std::string& toConstraint   = findNameByPointer(&c->getRigidBodyB());

    btRigidBody* a = nullptr;
    btRigidBody* b = nullptr;

    if (fromConstraint != "")
      a = elements.bodies[fromConstraint];

    if (toConstraint != "")
      b = elements.bodies[toConstraint];

    btTypedConstraint* constraintCopy = nullptr;

    switch (c->getConstraintType()) {
      case btTypedConstraintType::POINT2POINT_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for Point2Point constraint");
        break;
      case btTypedConstraintType::HINGE_CONSTRAINT_TYPE: {
        btHingeConstraint* h      = dynamic_cast<btHingeConstraint*>(c);
        const btTransform& aFrame = h->getAFrame();
        const btTransform& bFrame = h->getBFrame();
        btHingeConstraint* n = new btHingeConstraint(*a, *b, aFrame, bFrame);

        if (h->hasLimit()) {
          n->setLimit(h->getLowerLimit(), h->getUpperLimit());
        }

        mLog->debug("Constraint between A: '{}' -> B: {}", fromConstraint,
        toConstraint);

        const btVector3& aOrigin = aFrame.getOrigin();
        const btVector3& bOrigin = bFrame.getOrigin();

        mLog->debug("  PivotA: [{}, {}, {}]", aOrigin.x(), aOrigin.y(), aOrigin.z());
        mLog->debug("  PivotB: [{}, {}, {}]", bOrigin.x(), bOrigin.y(), bOrigin.z());

        const btVector3& axisRowA = aFrame.getBasis().getColumn(2);
        const btVector3& axisRowB = bFrame.getBasis().getColumn(2);

        mLog->debug("  AxisA: [{}, {}, {}]", axisRowA.x(), axisRowA.y(), axisRowA.z());
        mLog->debug("  AxisB: [{}, {}, {}]", axisRowB.x(), axisRowB.y(), axisRowB.z());

        if (h->hasLimit())
          mLog->debug("  hasLimit, lower: {}, upper: {}", h->getLowerLimit(),
          h->getUpperLimit());

        constraintCopy = n;
        break;
      }
      case btTypedConstraintType::CONETWIST_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for ConeTwist constraint");
        break;
      case btTypedConstraintType::D6_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for D6 constraint");
        break;
      case btTypedConstraintType::SLIDER_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for Slider constraint");
        break;
      case btTypedConstraintType::CONTACT_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for Contact constraint");
        break;
      case btTypedConstraintType::D6_SPRING_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for D6Spring constraint");
        break;
      case btTypedConstraintType::GEAR_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for Gear constraint");
        break;
      case btTypedConstraintType::FIXED_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for Gear constraint");
        break;
      case btTypedConstraintType::D6_SPRING_2_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for D6Spring2 constraint");
        break;
      case btTypedConstraintType::MAX_CONSTRAINT_TYPE:
        mLog->error("No duplication handler for Max constraint");
        break;
    }

    if (constraintCopy != nullptr) {
      elements.constraints[fromConstraint].push_back(constraintCopy);
      elements.constraints[toConstraint].push_back(constraintCopy);
    }
  }

  mCopies.push_back(elements);

  return &mCopies.back();
}

/**
 * @brief
 *   Clears all the resources that have been allocated to a PhysicsElement
 *   before removing it from the list of stored copies all together.
 *
 *   This will invalidate all other references to the PhysicsElement
 *
 * @param copy
 */
void PhysicsMesh::deleteCopy(PhysicsElements* copy) {
  unsigned int i = 0;
  for (auto& storedCopy : mCopies) {

    // when we have found our match, go through and delete it all
    if (&storedCopy == copy) {

      // Delete constraints first
      for (auto& el : storedCopy.constraints) {
        for (auto& innerEl : el.second) {
          if (storedCopy.bodies[el.first] == &innerEl->getRigidBodyA()) {
            delete innerEl;
            innerEl = nullptr;
          }
        }

        el.second.clear();
      }

      // Followed by all rigidBodies
      for (auto& el : storedCopy.bodies)
        delete el.second;

      for (auto& el : storedCopy.motions)
        delete el.second;


      // Now that all copies have been deleted, clear the maps
      // of strings.
      storedCopy.bodies.clear();
      storedCopy.motions.clear();
      storedCopy.constraints.clear();

      // Remove the element from the list and break the loop
      mCopies.erase(mCopies.begin() + i);
      break;
    }

    ++i;
  }
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
    SubMeshPhysics subMesh = findByName(name);

    if (subMesh.subMesh == nullptr) {
      continue;
    }

    if (subMesh.body == nullptr && subMesh.subMesh != nullptr &&
        subMesh.subMesh->size() == 0) {
      continue;
    }

    meshes.push_back(std::make_pair(name, findByName(name)));
  }

  return meshes;
}

const std::shared_ptr<Mesh>& PhysicsMesh::mesh() const {
  return mMesh;
}
