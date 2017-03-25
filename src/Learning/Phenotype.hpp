#pragma once

#include <mmm.hpp>

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
struct Phenotype {
  World*               world;
  Spider*              spider;
  NEAT::NeuralNetwork* network;
  mmm::vec<8>          fitness;

  Phenotype();
  ~Phenotype();

  // Resets the phenotype back into its original state
  void reset();

  // Performs the update of the phenotype
  void update(float deltaTime);
};
