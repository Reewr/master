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
    , planeMotion(nullptr)
    , planeBody(nullptr)
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
  delete planeMotion;
  delete planeBody;
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
  inputs.reserve(44);
  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {

      // vec3  ang = part.second.part->angularVelocity();
      float rot = part.second.hinge->getHingeAngle();

      // inputs.push_back(ang.x);
      // inputs.push_back(ang.y);
      // inputs.push_back(ang.z);
      inputs.push_back(rot - part.second.restAngle);

    } else if (part.second.dof != nullptr) {

      // TODO

    }
  }

  // activate network to retrieve output vector
  network->Flush();
  network->Input(inputs);
  network->Activate();
  std::vector<double> outputs = network->Output();

  // set hinge motor targets based on network output
  size_t i = 0;
  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {


    // part.second.hinge->enableAngularMotor(true, diff * 10.0, 1.0);

      auto currentAngle = part.second.hinge->getHingeAngle();
      auto targetAngle  = outputs[i] + part.second.restAngle;

      if (part.second.hinge->hasLimit()) {
        auto lowerLimit = part.second.hinge->getLowerLimit();
        auto upperLimit = part.second.hinge->getUpperLimit();
        targetAngle     = mmm::clamp(targetAngle, lowerLimit, upperLimit);
      }

      auto velocity     = (targetAngle - currentAngle) * 10.0f;
      part.second.hinge->enableAngularMotor(true, velocity, 1.f);

      i += 1;

    } else if (part.second.dof != nullptr) {

      // TODO

      i += 1;
    }
  }

  // update physics
  world->doPhysics(deltaTime);

  // update fitness if we can detect some state that we can judge fitness on

  // testing fitness...
  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {

      float r =
        mmm::abs(part.second.hinge->getHingeAngle() - part.second.restAngle);
      fitness[0] += 1.f / (r + 1.f);

    } else if (part.second.dof != nullptr) {

      // TODO

    }
  }

  fitness[0] /= float(i);
}

void Phenotype::reset() {
  if (world == nullptr)
    world = new World(mmm::vec3(0, -9.81, 0));
  else
    world->reset();

  if (planeBody == nullptr) {
    planeMotion = new btDefaultMotionState(
      btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo consInfo(
      0, planeMotion, plane, btVector3(0, 0, 0));
    planeBody = new btRigidBody(consInfo);
    world->world()->addRigidBody(planeBody);
  }

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

btStaticPlaneShape* Phenotype::plane =
  new btStaticPlaneShape(btVector3(0, 1, 0), 1);
