#include "Phenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>
#include <btBulletDynamicsCommon.h>

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"

using mmm::vec3;

Phenotype::Phenotype()
    : Logging::Log("Phenotype")
    , world(nullptr)
    , spider(nullptr)
    , network(nullptr)
    , fitness(1) {}

Phenotype::~Phenotype() {}

/**
 * @brief
 *   Deletes the memory allocated for the phenotype.
 */
void Phenotype::remove() {
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

  // construct input vector which is known to be exactly 172 elements
  std::vector<double> inputs;
  inputs.reserve(172);
  for (auto& part : spider->parts()) {
    if (part.second.hinge == nullptr)
      continue;

    vec3  ang = part.second.part->angularVelocity();
    float rot = part.second.hinge->getHingeAngle();

    inputs.push_back(ang.x);
    inputs.push_back(ang.y);
    inputs.push_back(ang.z);
    inputs.push_back(rot - part.second.restAngle);
  }

  // activate network to retrieve output vector
  network->Flush();
  network->Input(inputs);
  network->Activate();
  std::vector<double> outputs = network->Output();

  // set hinge motor targets based on network output
  size_t i = 0;
  for (auto& part : spider->parts()) {
    if (part.second.hinge == nullptr)
      continue;

    // part.second.hinge->enableAngularMotor(true, 0, 100);
    // part.second.hinge->setMotorTarget(part.second.restAngle, deltaTime);

    part.second.hinge->enableAngularMotor(true, 0, 100);
    part.second.hinge->setMotorTarget(outputs[i] + part.second.restAngle,
                                      deltaTime);
    i += 1;
  }

  // update physics
  world->doPhysics(deltaTime);

  // update fitness if we can detect some state that we can judge fitness on

  // testing fitness...
  for (auto& part : spider->parts()) {
    if (part.second.hinge == nullptr)
      continue;

    float rot =
      mmm::abs(part.second.hinge->getHingeAngle() + part.second.restAngle);
    fitness[0] *= 1 / (rot + 1);
  }
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

  fitness = mmm::vec<8>(1);
}
