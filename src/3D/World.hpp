#pragma once

#include <vector>
#include <mmm.hpp>

class btDbvtBroadphase;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class Drawable;

class World {
public:
  World(const mmm::vec3& gravity);
  ~World();

  // adds an element to the physics world
  void addObject(Drawable* element);

  // removes an object. Removes all that are equal if there
  // are multiple which are
  void removeObject(Drawable* element = nullptr);

  // does the physics!
  // also tells all drawable elements to update their position
  void doPhysics(float deltaTime);

private:

  btDbvtBroadphase*                    phase;
  btSequentialImpulseConstraintSolver* solver;
  btDefaultCollisionConfiguration*     collision;
  btCollisionDispatcher*               dispatcher;
  btDiscreteDynamicsWorld*             world;

  std::vector<Drawable*> mElements;
};
