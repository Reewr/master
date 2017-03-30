#pragma once

#include "../Log.hpp"

#include <mmm.hpp>
#include <vector>

class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btPoint2PointConstraint;
class btRigidBody;
class btSequentialImpulseConstraintSolver;
class btMLCPSolverInterface;
class btBroadphaseInterface;
struct btOverlapFilterCallback;

class Drawable3D;
class Camera;

namespace Input {
  class Event;
}

class World : public Logging::Log {
public:

  enum class Solver {
    Standard,
    Dantzig,
    Lemke,
    ProjectedGaussSeidel
  };

  enum class Broadphase {
    Dbvt,
    AxisSweep
  };

  World(const mmm::vec3& gravity,
        Solver solver = Solver::Standard,
        Broadphase phase = Broadphase::Dbvt);
  ~World();

  // adds an element to the physics world
  void addObject(Drawable3D* element);

  // removes an object. Removes all that are equal if there
  // are multiple which are
  void removeObject(Drawable3D* element = nullptr);

  // does the physics!
  // also tells all drawable elements to update their position
  void doPhysics(float deltaTime);

  // By enabling mousepicks, the user can move objects by clicking
  // on them to pick them up and dragging them around
  void enableMousePickups();

  // Sets a filter for collision detection allowing you to have
  // better control over what the world checks collisions on.
  void setCollisionFilter(btOverlapFilterCallback* callback);

  // By disabling mousepickups, the user is not allowed to move objects
  void disableMousePickups();

  // Disables physics, stopping all components from moving
  void disablePhysics();

  // Enables physics again, resuming movement of compontents
  void enablePhysics();

  // Resets the world, resetting all caches
  void reset();

  // The function that handles input events
  void input(Camera* camera, const Input::Event& event);

  btDiscreteDynamicsWorld* world();

private:
  bool pickBody(const mmm::vec3& rayFromWorld, const mmm::vec3& rayToWorld);

  bool movePickedBody(const mmm::vec3& rayFromWorld,
                      const mmm::vec3& rayToWorld);

  void removePickingConstraint();

  btBroadphaseInterface*               mPhase;
  btSequentialImpulseConstraintSolver* mSolver;
  btDefaultCollisionConfiguration*     mCollision;
  btCollisionDispatcher*               mDispatcher;
  btDiscreteDynamicsWorld*             mWorld;
  btMLCPSolverInterface*               mSolverInterface;

  // Mouse pickup variables
  bool                     mHasMousePickup;
  btRigidBody*             mPickedBody;
  btPoint2PointConstraint* mPickedConstraint;
  mmm::vec3                mOldPickingPos;
  mmm::vec3                mHitPos;
  float                    mOldPickingDistance;
  int                      mSavedState;

  std::vector<Drawable3D*> mElements;
};
