#include "World.hpp"

#include "../Drawable/Drawable3D.hpp"
#include <algorithm>
#include <btBulletDynamicsCommon.h>

using mmm::vec3;

/**
 * @brief
 *   Creates a new physics world with the specified gravity
 *
 * @param gravity
 */
World::World(const vec3& gravity) {
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
  world->stepSimulation(deltaTime, 1, 1.0 / 144.0);

  for (auto a : mElements)
    a->updateFromPhysics();
}
