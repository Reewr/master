#pragma once

#include "../Log.hpp"

#include <mmm.hpp>
#include <vector>

struct btDbvtBroadphase;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btPoint2PointConstraint;

class Drawable3D;
class Camera;

namespace Input {
  class Event;
}

class World : public Logging::Log {
public:
  World(const mmm::vec3& gravity);
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

  // By disabling mousepickups, the user is not allowed to move objects
  void disableMousePickups();

  // The function that handles input events
  void input(Camera* camera, const Input::Event& event);

private:
  bool pickBody(const mmm::vec3& rayFromWorld, const mmm::vec3& rayToWorld);

  bool movePickedBody(const mmm::vec3& rayFromWorld,
                      const mmm::vec3& rayToWorld);

  void removePickingConstraint();

  btDbvtBroadphase*                    mPhase;
  btSequentialImpulseConstraintSolver* mSolver;
  btDefaultCollisionConfiguration*     mCollision;
  btCollisionDispatcher*               mDispatcher;
  btDiscreteDynamicsWorld*             mWorld;

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
