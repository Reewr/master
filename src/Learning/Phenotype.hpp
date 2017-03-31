#pragma once

#include <mmm.hpp>

#include "../Log.hpp"

struct btDefaultMotionState;
class btRigidBody;
class btStaticPlaneShape;

class World;
class Spider;

namespace NEAT {
  class NeuralNetwork;
  class Genome;
}

/**
 * A Phenotype represents a single individual in the total population. When
 * the SpiderSwarm is initialized, a set amount of Phenotypes are created,
 * each containing their own Spider, World and Network.
 *
 * The reason for having one world per spider is due to the complexity
 * of the Spider. Even by setting up CollisionGroups and ignoring collisions
 * between spiders, the performance is slower than having one world per
 * spider. The downside of this approach is that it'll require more memory
 */
struct Phenotype : Logging::Log {
  World*               world;
  Spider*              spider;
  NEAT::NeuralNetwork* network;

  btDefaultMotionState*      planeMotion;
  btRigidBody*               planeBody;
  static btStaticPlaneShape* plane;

  mmm::vec<8> fitness;
  int         numUpdates;

  Phenotype();
  ~Phenotype();

  // Returns the final fitness of the Phenotype
  float finalizeFitness();

  // Deletes the memory allocated in reset
  void remove();

  // Resets the phenotype back into its original state
  void reset();

  // Performs the update of the phenotype
  void update(float deltaTime);
};
