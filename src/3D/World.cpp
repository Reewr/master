#include "World.hpp"

#include "../Drawable/Drawable3D.hpp"
#include "../Input/Event.hpp"
#include <algorithm>
#include <btBulletDynamicsCommon.h>

using mmm::vec3;

/**
 * @brief
 *   Creates a new physics world with the specified gravity
 *
 * @param gravity
 */
World::World(const vec3& gravity)
    : Logging::Log("World")
    , mHasMousePickup(false)
    , mPickedBody(nullptr)
    , mPickedConstraint(nullptr)
    , mOldPickingPos(0, 0, 0)
    , mHitPos(0, 0, 0)
    , mOldPickingDistance(0) {
  collision  = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision);
  phase      = new btDbvtBroadphase();
  solver     = new btSequentialImpulseConstraintSolver;

  world = new btDiscreteDynamicsWorld(dispatcher, phase, solver, collision);
  world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

World::~World() {
  for (auto element : mElements)
    world->removeRigidBody(element->rigidBody());

  delete world;
  delete solver;
  delete collision;
  delete dispatcher;
  delete phase;

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
  if (!element->hasPhysics())
    return;

  world->addRigidBody(element->rigidBody());
  mElements.push_back(element);
}

/**
 * @brief
 *   Removes the element(s) that are equal to element. If
 *   element is a nullptr (which it is by default) it will
 *   remove all elements that are nullptrs
 *
 * @param element the drawable element to remove
 */
void World::removeObject(Drawable3D* element) {
  // remove them from the world first
  for (auto a : mElements) {
    if (a == element && element != nullptr)
      world->removeRigidBody(a->rigidBody());
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
void World::doPhysics(float deltaTime) {
  world->stepSimulation(deltaTime, 7);

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
 *   Handles the input events. Currently it will only handle the event
 *   where the user tries to pickup an object and nove it around.
 *
 * @param event
 */
void World::input(Input::Event& event) { }


bool World::pickBody(const mmm::vec3& rayFromWorld,
                             const mmm::vec3& rayToWorld) {
  btVector3 rfw = btVector3(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z);
  btVector3 rtw = btVector3(rayToWorld.x, rayToWorld.y, rayToWorld.z);

  btCollisionWorld::ClosestRatResultCallback rayCallback(rfw, rtw);

  mWorld->rayTest(rfw, rtw, rayCallback);

  if (!rayCallback.hasHit())
    return false;

  btVector3 pickPos = rayCallback.m_hitPointWorld;
  btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);

  if (body && !body->isStaticObject() && !body->isKinematicObject()) {
    mPickedBody = body;
    mSavedState = mPickedBody->getActivationState();
    mPickedBody->setActivationState(DISABLE_DEACTIVATION);

    btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
    mPickedConstraint    = new btPoint2PointConstraint(*body, localPivot);
    mWorld->addConstraint(mPickedConstraint, true);

    btScalar mousePickClamping = 30.0f;
    mPickedConstraint->m_setting.m_impulseClamp = mousePickClamping;
    mPickedConstraint->m_setting.m_tau = 0.001f;
  }

  mOldPickingPos = rayFromWorld;
  mHitPos = mmm::vec3(pickPos.x(), pickPos.y(), pickPos.z());
  mOldPickingDistance = (mHitPos - rayFromWorld).length();

  return true;
}
