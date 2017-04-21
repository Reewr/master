#include "World.hpp"

#include "../Camera/Camera.hpp"
#include "../Drawable/Drawable3D.hpp"
#include "../Input/Event.hpp"
#include "../OpenGLHeaders.hpp"
#include <BulletDynamics/MLCPSolvers/btDantzigSolver.h>
#include <BulletDynamics/MLCPSolvers/btLemkeSolver.h>
#include <BulletDynamics/MLCPSolvers/btMLCPSolver.h>
#include <BulletDynamics/MLCPSolvers/btSolveProjectedGaussSeidel.h>
#include <algorithm>
#include <assert.h>
#include <btBulletDynamicsCommon.h>

using mmm::vec3;

/**
 * @brief
 *   Creates a new physics world with the specified gravity
 *
 * @param gravity
 * @param solver
 * @param phase
 */
World::World(const vec3& gravity, World::Solver solver, World::Broadphase phase)
    : Logging::Log("World")
    , mSolverInterface(nullptr)
    , mHasMousePickup(false)
    , mPickedBody(nullptr)
    , mPickedConstraint(nullptr)
    , mOldPickingPos(0, 0, 0)
    , mHitPos(0, 0, 0)
    , mOldPickingDistance(0) {
  mCollision  = new btDefaultCollisionConfiguration();
  mDispatcher = new btCollisionDispatcher(mCollision);
  mPhase      = new btDbvtBroadphase();

  switch (solver) {
    case Solver::Standard:
      mSolver = new btSequentialImpulseConstraintSolver;
      break;
    case Solver::Dantzig:
      mSolverInterface = new btDantzigSolver();
      mSolver          = new btMLCPSolver(mSolverInterface);
      break;
    case Solver::Lemke:
      mSolverInterface = new btLemkeSolver();
      mSolver          = new btMLCPSolver(mSolverInterface);
      break;
    case Solver::ProjectedGaussSeidel:
      mSolverInterface = new btSolveProjectedGaussSeidel();
      mSolver          = new btMLCPSolver(mSolverInterface);
      break;
  }

  switch (phase) {
    case Broadphase::Dbvt:
      mPhase = new btDbvtBroadphase();
      break;
    case Broadphase::AxisSweep:
      mPhase = new btAxisSweep3(btVector3(-1000, -1000, -1000),
                                btVector3(1000, 1000, 1000));
  }

  mWorld =
    new btDiscreteDynamicsWorld(mDispatcher, mPhase, mSolver, mCollision);
  mWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

  //
  // Configure solver
  //

  btContactSolverInfo& info = mWorld->getSolverInfo();

  // info.m_tau = 0.6;
  // info.m_damping = 1;
  info.m_friction = 0.3;
  // info.m_restitution = 0;
  info.m_numIterations = 25;
  // info.m_maxErrorReduction = 20;
  // info.m_sor = 1;
  info.m_erp = 0.8;
  // info.m_erp2 = 0.2;

  info.m_globalCfm = 0.00001; // default = 0

  // info.m_splitImpulse = 1;
  // info.m_splitImpulsePenetrationThreshold = -0.04;
  // info.m_splitImpulseTurnErp = 0.1;
  // info.m_linearSlop = 0;
  // info.m_warmstartingFactor = 0.85;
  // info.m_solverMode = 260;
  // info.m_restingContactRestitutionThreshold = 2;

  // If using a MLCP solver it is better to have a small A matrix according
  // to Bullet docs
  //
  // Otherwise have a large batch size since small batches have larger overhead
  // btSequentialImpulseConstraintSolver
  info.m_minimumSolverBatchSize = (solver != Solver::Standard) ? 1 : 128;

  // info.m_minimumSolverBatchSize = 128;
  // info.m_maxGyroscopicForce = 100;
  // info.m_singleAxisRollingFrictionThreshold = 1e30;
}

World::~World() {
  removePickingConstraint();

  delete mWorld;
  delete mSolver;
  delete mSolverInterface;
  delete mCollision;
  delete mDispatcher;
  delete mPhase;

  mElements.clear();
}

/**
 * @brief
 *   Adds the element to the physics world. It is expected to
 *   return true on a `hasPhysics()` call. If this is the case,
 *   it is also added to a vector stored within the World class
 *
 * @param element
 */
void World::addObject(Drawable3D* element) {
  if (element->hasPhysics()) {
    btRigidBody* body = element->rigidBody();

    if (body->getInvMass() == 0)
      mWorld->addRigidBody(body, 1, -1);
    else
      mWorld->addRigidBody(body,
                           element->collisionGroup(),
                           element->collisionMask());

    for (auto& c : element->constraints()) {
      if (&c->getRigidBodyA() == body) {
        mWorld->addConstraint(c);
      }
    }
  }

  mElements.push_back(element);

  for (auto& child : element->children())
    addObject(child);
}

/**
 * @brief
 *   Resets all the information of the world
 */
void World::reset() {

  // No internal values should be nullptr
  assert(mPhase != nullptr);
  assert(mWorld != nullptr);
  assert(mSolver != nullptr);
  assert(mDispatcher != nullptr);

  btOverlappingPairCache* pairCache = mPhase->getOverlappingPairCache();

  if (pairCache != nullptr) {
    btBroadphasePairArray& pairArray = pairCache->getOverlappingPairArray();

    for (int i = 0; i < pairArray.size(); ++i)
      pairCache->cleanOverlappingPair(pairArray[i], mDispatcher);
  }

  mSolver->reset();
  mWorld->clearForces();
  mPhase->resetPool(mDispatcher);
}

/**
 * @brief
 *   Removes the element(s) that are equal to element. If
 *   element is a nullptr (which it is by default) it will
 *   remove all elements that are nullptrs
 *
 * @param element the drawable element to remove
 * @param removeChildren
 *   if this is true, which it defaults to, it will remove
 *   children as well.
 */
void World::removeObject(Drawable3D* element, bool removeChildren) {
  if (element == nullptr)
    return;

  // remove them from the world first
  for (auto a : mElements) {
    if (a == element && element != nullptr) {

      if (a->hasPhysics()) {
        mWorld->removeRigidBody(a->rigidBody());

        for (auto& c : a->constraints()) {
          if (&c->getRigidBodyA() == a->rigidBody()) {
            mWorld->removeConstraint(c);
          }
        }
      }

      if (removeChildren) {
        for (auto& child : a->children())
          removeObject(child);
      }
    }
  }

  // then remove them from the list
  mElements.erase(std::remove_if(mElements.begin(),
                                 mElements.end(),
                                 [element](Drawable* e) {
                                   return e == element;
                                 }),
                  mElements.end());
}

/**
 * @brief
 *   Does physics!
 *
 *   Steps over the simulation with deltaTime being the step
 *   time. Also tells all physics objects to update their
 *   positions.
 *
 * @param deltaTime
 */
void World::doPhysics(float) {
  mWorld->stepSimulation(1.f/60.f, 3, 1.f/120.f);

  for (auto a : mElements)
    a->updateFromPhysics();
}

/**
 * @brief
 *   When called, it will start to handle inputs that are sent to the
 *   input handler and check to see if the user tries to pick up any of
 *   the physics elements.
 *
 *   If a user clicks on a physics element that is not static and can be moved
 *   the world will add a constraint between the object and the mouse and
 *   have it follow the mouse pointer.
 */
void World::enableMousePickups() {
  mHasMousePickup = true;
}

/**
 * @brief
 *   Disables picking up of object by mouse
 */
void World::disableMousePickups() {
  mHasMousePickup = false;
}

/**
 * @brief
 *   Sets the collision filter for the world. When this is called it overrides
 *   whatever was set before it and the default if there's none set.
 *
 *   This allows you to better control the collisions detections that are
 *   performed in the world.
 *
 * @param callback
 */
void World::setCollisionFilter(btOverlapFilterCallback* callback) {
  mWorld->getPairCache()->setOverlapFilterCallback(callback);
}

/**
 * @brief
 *   Handles the input events. Currently it will only handle the event
 *   where the user tries to pickup an object and nove it around.
 *
 * @param camera
 * @param event
 */
void World::input(Camera* camera, const Input::Event& event) {
  if (event.buttonPressed(GLFW_MOUSE_BUTTON_1)) {

    const mmm::vec3& rayFrom = camera->position();
    const mmm::vec3& rayTo   = camera->screenPointToRay(event.position());

    mLog->debug("Picked: RayFrom '{}', RayTo: '{}'", rayFrom, rayTo);

    bool hit = pickBody(rayFrom, rayTo);

    if (hit) {
      event.stopPropgation();
      mLog->debug("Hit an object: {}",
                  mPickedBody == nullptr ? "nullptr" : "not nullptr");
    }

    return;
  } else if (event == Input::Event::Type::MouseRelease) {
    removePickingConstraint();
    return;
  }

  if (event == Input::Event::Type::MouseMovement && mPickedBody) {
    movePickedBody(camera->position(),
                   camera->screenPointToRay(event.position()));
  }
}

btDiscreteDynamicsWorld* World::world() {
  return mWorld;
}

/**
 * @brief
 *   When the user clicks the mouse, this function is called to see whether
 *   the mouse pointer clicks on any physics objects in the world.
 *
 *   If an object is hit, its set as the picked object. A constraint
 *   between the mouse pointer and the object is added so that whenever
 *   the user moves the mouse (as long as the mousebutton is still pressed)
 *   the object moves along with it.
 *
 * @param rayFromWorld
 * @param rayToWorld
 *
 * @return
 */
bool World::pickBody(const mmm::vec3& rayFromWorld,
                     const mmm::vec3& rayToWorld) {
  btVector3 rfw = btVector3(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z);
  btVector3 rtw = btVector3(rayToWorld.x, rayToWorld.y, rayToWorld.z);

  btCollisionWorld::ClosestRayResultCallback rayCallback(rfw, rtw);

  mWorld->rayTest(rfw, rtw, rayCallback);

  if (!rayCallback.hasHit())
    return false;

  btVector3 pickPos = rayCallback.m_hitPointWorld;
  mLog->debug("PickedPos: '{}'",
              mmm::vec3(pickPos.x(), pickPos.y(), pickPos.z()));

  btRigidBody* body =
    (btRigidBody*) btRigidBody::upcast(rayCallback.m_collisionObject);

  if (body && !body->isStaticObject() && !body->isKinematicObject()) {
    mLog->debug("Hit a body that is not static or kinematic!");

    mPickedBody = body;
    mSavedState = mPickedBody->getActivationState();
    mPickedBody->setActivationState(DISABLE_DEACTIVATION);

    btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
    mPickedConstraint    = new btPoint2PointConstraint(*body, localPivot);
    mWorld->addConstraint(mPickedConstraint, true);

    btScalar mousePickClamping                  = 30.0f;
    mPickedConstraint->m_setting.m_impulseClamp = mousePickClamping;
    mPickedConstraint->m_setting.m_tau          = 0.01f;
  } else if (body) {
    mLog->debug("Hit static object");
  }

  mOldPickingPos      = rayToWorld;
  mHitPos             = mmm::vec3(pickPos.x(), pickPos.y(), pickPos.z());
  mOldPickingDistance = mmm::length(mHitPos - rayFromWorld);

  return true;
}

/**
 * @brief
 *   Disables physics on every element within the world
 */
void World::disablePhysics() {
  for (auto& e : mElements) {
    if (e->hasPhysics())
      e->rigidBody()->forceActivationState(WANTS_DEACTIVATION);
  }
}

/**
 * @brief
 *   Enables physics on every element within the world
 */
void World::enablePhysics() {
  for (auto& e : mElements) {
    if (e->hasPhysics())
      e->rigidBody()->forceActivationState(DISABLE_DEACTIVATION);
  }
}

/**
 * @brief
 *   If the user has clicked on an object with the mouse and starts
 *   to move the mouse, the object should be moved. This handles that movement.
 *
 * @param rayFromWorld
 * @param rayToWorld
 *
 * @return
 */
bool World::movePickedBody(const mmm::vec3& rayFromWorld,
                           const mmm::vec3& rayToWorld) {

  if (!mPickedBody || !mPickedConstraint)
    return false;

  mmm::vec3 dir   = normalize(rayToWorld - rayFromWorld) * mOldPickingDistance;
  mmm::vec3 pivot = rayFromWorld + dir;
  mPickedConstraint->setPivotB(btVector3(pivot.x, pivot.y, pivot.z));

  return true;
}

/**
 * @brief
 *   When the mouse button is released, the constraint should be cleared and
 *   the item in question should be dropped. This function handles that
 */
void World::removePickingConstraint() {
  if (!mPickedConstraint)
    return;

  mPickedBody->forceActivationState(mSavedState);
  mPickedBody->activate();
  mWorld->removeConstraint(mPickedConstraint);
  delete mPickedConstraint;
  mPickedConstraint = nullptr;
  mPickedBody       = nullptr;
}
