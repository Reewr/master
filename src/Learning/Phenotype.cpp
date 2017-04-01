#include "Phenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>
#include <btBulletDynamicsCommon.h>

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"

using mmm::vec3;

/**
 * @brief
 *   Returns the Euler angles of a Quaternion
 *
 * @param v
 *
 * @return
 */
mmm::vec3 getEulerAngles(mmm::vec4 v) {
  double    sqw  = v.w * v.w;
  double    sqx  = v.x * v.x;
  double    sqy  = v.y * v.y;
  double    sqz  = v.z * v.z;
  double    unit = sqx + sqy + sqz + sqw;
  double    test = v.x * v.y + v.z * v.w;
  mmm::vec3 r;

  if (test > 0.499 * unit) { // singularity at north pole
    r.z = 2 * atan2(v.x, v.w);
    r.x = mmm::constants<double>::pi / 2;
    r.y = 0;
    return r;
  }
  if (test < -0.499 * unit) { // singularity at south pole
    r.z = -2 * atan2(v.x, v.w);
    r.x = -mmm::constants<double>::pi / 2;
    r.y = 0;
    return r;
  }
  r.z = atan2(2.0 * v.y * v.w - 2.0 * v.x * v.z, sqx - sqy - sqz + sqw);
  r.x = asin(2.0 * test / unit);
  r.y = atan2(2.0 * v.x * v.w - 2 * v.y * v.z, -sqx + sqy - sqz + sqw);

  return r;
}

Phenotype::Phenotype()
    : Logging::Log("Phenotype")
    , world(nullptr)
    , spider(nullptr)
    , network(nullptr)
    , planeMotion(nullptr)
    , planeBody(nullptr)
    , fitness(1)
    , numUpdates(0)
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
  // network->Flush();
  network->Input(inputs);
  network->Activate();
  std::vector<double> outputs = network->Output();

  // set hinge motor targets based on network output
  size_t i = 0;
  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {

      // part.second.hinge->enableAngularMotor(true, diff * 10.0, 1.0);

      auto currentAngle = part.second.hinge->getHingeAngle();
      auto targetAngle  = part.second.restAngle < 0 ? -outputs[i] : outputs[i];
      //+ part.second.restAngle;

      auto velocity = (targetAngle - currentAngle) * 10.0f;
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
  float maxFitnessAngle = 0;

  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {

      float r =
        mmm::abs(part.second.hinge->getHingeAngle() - part.second.restAngle);
      maxFitnessAngle += 1.f / (r + 1.f);

    } else if (part.second.dof != nullptr) {

      // TODO
    }
  }

  // The below fitness calculation are purely for testing to see what manner
  // of insane spiders we can get. Mostly crabs now.
  //
  // The below always gives a number between 0 and 1 and is added
  // to each of the different fitness values. When doing the final
  // calculations, all values are divided by the number of updates
  // to give an average over the time of the simulation.
  //
  // The latter calculation forces the robot to do well during the entire
  // simulation instead of just at the end of it.
  //
  // See `finalizeFitness` for more information

  btRigidBody* sternum  = spider->parts()["Sternum"].part->rigidBody();
  float        pos      = sternum->getCenterOfMassPosition().z();
  float        ySternum = sternum->getCenterOfMassPosition().y();
  btVector3    lVelocity = sternum->getLinearVelocity();
  float        zVelocity = lVelocity.safeNormalize().z();

  // Calculate the fitness based on the angle of its hinges, dividing the
  // accumlicated value by the number of hinges
  fitness[0] += mmm::clamp(maxFitnessAngle / float(i), 0.0, 1.0);

  // Calculate the fitness based on the position of the sternum. The closer
  // to 5z the better.
  fitness[1] += mmm::clamp(1.0 / (mmm::abs(pos - 5) + 1), 0.0, 1.0);

  // Calculate the fitness based on the height of the sternum. The closer to
  // 1.5y, the better.
  fitness[2] += mmm::clamp(1.0 / (mmm::abs(ySternum - 1.0) + 1), 0.0, 1.0);

  // Calculate the fitness based on the z-velocity of the sternum. The closer
  // to 1 the better.
  fitness[3] += mmm::clamp(1.0 / (mmm::abs(zVelocity - 1.0) + 1), 0.0, 1.0);

  ++numUpdates;
}

/**
 * @brief
 *   Makes the final calculations of the Phenotype, returning the fitness
 *   as a floating point value
 *
 * @return
 */
float Phenotype::finalizeFitness() {
  float updates = numUpdates;
  fitness[0] /= updates;
  fitness[1] /= updates;
  fitness[2] /= updates;
  fitness[3] /= updates;

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

  numUpdates = 0;
  fitness    = mmm::vec<8>(1);
  speciesIndex = speciesId;
  individualIndex = individualId;
}

btStaticPlaneShape* Phenotype::plane =
  new btStaticPlaneShape(btVector3(0, 1, 0), 1);
