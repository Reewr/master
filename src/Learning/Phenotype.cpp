#include "Phenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>

#include "../3D/World.hpp"
#include "../3D/Spider.hpp"

Phenotype::Phenotype()
  : world(nullptr)
  , spider(nullptr)
  , network(nullptr)
  , fitness(0) {}

Phenotype::~Phenotype() {
  delete world;
  delete spider;
  delete network;
}

/**
 * @brief
 *   Activates the network associated with the spider by using
 *   positions, velocity etc from each of the components of the spider,
 *   generating a value as torque.
 *   It then performs a physics simulation on the spider before updating
 *   the fitness.
 *
 * @param deltaTime
 */
void Phenotype::update(float deltaTime) {
  // activate network for each spider given current motion state
  world->doPhysics(deltaTime);
  // update fitness if we can detect some state that we can judge fitness on
}

void Phenotype::reset() {
  if (world == nullptr)
    world = new World(mmm::vec3(0, -9.81, 0));
  else
    world->reset();

  if (spider == nullptr) {
    spider = new Spider();
    world->addObject(spider);
  } else {
    spider->reset();
  }

  if (network == nullptr)
    network = new NEAT::NeuralNetwork();
  else
    network->Clear();
}
