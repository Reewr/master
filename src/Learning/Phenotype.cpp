#include "Phenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>
#include <btBulletDynamicsCommon.h>

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"

#include "DrawablePhenotype.hpp"

using mmm::vec2;
using mmm::vec3;

/**
 * @brief
 *   Returns the Euler angles of a Quaternion
 *
 * @param v
 *
 * @return
 */
mmm::vec3 getEulerAngles(float x, float y, float z, float w) {
  double    sqw  = w * w;
  double    sqx  = x * x;
  double    sqy  = y * y;
  double    sqz  = z * z;
  double    unit = sqx + sqy + sqz + sqw;
  double    test = x * y + z * w;
  mmm::vec3 r;

  if (test > 0.499 * unit) { // singularity at north pole
    r.z = 2.0 * atan2(x, w);
    r.x = mmm::constants<double>::pi / 2.0;
    r.y = 0.0;
    return r;
  }
  if (test < -0.499 * unit) { // singularity at south pole
    r.z = -2.0 * atan2(x, w);
    r.x = -mmm::constants<double>::pi / 2.0;
    r.y = 0.0;
    return r;
  }
  r.z = atan2(2.0 * y * w - 2.0 * x * z, sqx - sqy - sqz + sqw);
  r.x = asin(2.0 * test / unit);
  r.y = atan2(2.0 * x * w - 2.0 * y * z, -sqx + sqy - sqz + sqw);

  return r;
}

Phenotype::Phenotype()
    : Logging::Log("Phenotype")
    , world(nullptr)
    , spider(nullptr)
    , network(nullptr)
    , planeMotion(nullptr)
    , planeBody(nullptr)
    , drawablePhenotype(nullptr)
    , fitness(1)
    , numUpdates(0)
    , failed(false)
    , duration(0)
    , speciesIndex(-1)
    , individualIndex(-1) {}

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

  if (failed)
    return;

  auto pi = mmm::constants<float>::pi;

  // construct input vector which is known to be exactly 172 elements
  std::vector<double> inputs;
  inputs.reserve(52);

  float phase0 = mmm::sin(duration);
  float phase1 = mmm::sin(duration + pi * 0.5);
  float phase2 = mmm::sin(duration + pi);
  float phase3 = mmm::sin(duration + pi * 1.5);

  inputs.push_back(phase0);
  inputs.push_back(phase1);
  inputs.push_back(phase2);
  inputs.push_back(phase3);
  inputs.push_back(phase2);
  inputs.push_back(phase3);
  inputs.push_back(phase0);
  inputs.push_back(phase1);

  duration += deltaTime;

  for (auto& part : spider->parts()) {

    // vec3  ang = part.second.part->angularVelocity();
    // inputs.push_back(ang.x);
    // inputs.push_back(ang.y);
    // inputs.push_back(ang.z);

    // auto ypos = part.second.part->rigidBody()->getCenterOfMassPosition().y();
    // inputs.push_back(ypos);

    if (part.second.hinge != nullptr) {

      float rot = part.second.hinge->getHingeAngle();
      inputs.push_back(rot - part.second.restAngle);

    } else if (part.second.dof != nullptr) {

      // TODO
    }
  }

  // activate network to retrieve output vector
  // network->Flush();
  network->Input(inputs);
  network->Activate();
  std::vector<double> outputs = network->Output();

  // set hinge motor targets based on network output
  size_t i = 0;
  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {

      auto currentAngle = part.second.hinge->getHingeAngle();
      auto targetAngle  = outputs[i] * 4.f * pi - 2.f * pi + part.second.restAngle;
      auto velocity     = (targetAngle - currentAngle) * 40.0f;
      part.second.hinge->enableAngularMotor(true, velocity, 4.f);

      i += 1;

    } else if (part.second.dof != nullptr) {

      // TODO

      i += 1;
    }
  }

  // update physics
  world->doPhysics(deltaTime);

  // judge fitness, if we can
  updateFitness(deltaTime);

  // update counter
  ++numUpdates;
}

float score(float deltaTime, float zeroIsBest, float bias = 0.05f) {
  return 1.f / (mmm::max(mmm::abs(zeroIsBest) - bias, 0.f) * deltaTime + 1.f);
};

void Phenotype::updateFitness(float deltaTime) {
  auto& parts = spider->parts();

  { // Stability - Fail if sternum rotates in any direction above 60 degrees
    auto q = parts["Sternum"].part->rigidBody()->getOrientation();
    vec3 r = mmm::degrees(getEulerAngles(q.x(), q.y(), q.z(), q.w()));
    r.y += 90;

    if (mmm::any(mmm::greaterThan(r, 60))) {
      failed = true;
      return;
    }
  }
}

/**
 * @brief
 *   Makes the final calculations of the Phenotype, returning the fitness
 *   as a floating point value
 *
 * @return
 */
float Phenotype::finalizeFitness() {
  if (failed)
    return 0.f;

  // length walked
  // btRigidBody* sternum = spider->parts()["Sternum"].part->rigidBody();
  // auto z = sternum->getCenterOfMassPosition().z();
  // fitness[8] = z;

  return mmm::product(fitness);
}

/**
 * @brief
 *   Resets the Phenotype by resetting the values
 *   on the world, spider and network. Also resets
 *   the fitness and number of updates.
 *
 *   If all the values are nullptr, allocate them
 */
void Phenotype::reset(int speciesId, int individualId) {
  if (world == nullptr)
    world = new World(mmm::vec3(0, -9.81, 0));
  else
    world->reset();

  if (drawablePhenotype == nullptr)
    drawablePhenotype = new DrawablePhenotype();

  if (planeBody == nullptr) {
    planeMotion = new btDefaultMotionState(
      btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo consInfo(0,
                                                      planeMotion,
                                                      plane,
                                                      btVector3(0, 0, 0));
    planeBody = new btRigidBody(consInfo);
    world->world()->addRigidBody(planeBody);
  }

  if (spider == nullptr) {
    spider = new Spider();
    world->addObject(spider);
    world->enablePhysics();
  } else {
    spider->reset();
  }

  if (network == nullptr)
    network = new NEAT::NeuralNetwork();
  else {
    network->Clear();
    network->Flush();
  }

  failed     = false;
  duration   = 0;
  numUpdates = 0;
  fitness    = mmm::vec<9>(1);
  speciesIndex = speciesId;
  individualIndex = individualId;
}

btStaticPlaneShape* Phenotype::plane =
  new btStaticPlaneShape(btVector3(0, 1, 0), 1);
