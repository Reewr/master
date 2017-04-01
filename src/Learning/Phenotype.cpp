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
      inputs.push_back(rot);

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
      auto pi           = mmm::constants<float>::pi;
      auto targetAngle  = outputs[i] * 4.f * pi - 2.f * pi;

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

  // judge fitness, if we can
  updateFitness(deltaTime);

  // update counter
  ++numUpdates;
}

void Phenotype::updateFitness(float deltaTime) {

  // { // Stability
  //   btRigidBody* sternum  = spider->parts()["Sternum"].part->rigidBody();

  //   // height of the sternum
  //   float h = sternum->getCenterOfMassPosition().y() - 0.75f;

  //   // rotation of the sternum in the xz plane
  //   auto  q = sternum->getOrientation();
  //   vec3  r = getEulerAngles(q.x(), q.y(), q.z(), q.w());

  //   // fitness[0] *= 1.f / (mmm::abs(h) * deltaTime + 1.f);
  //   // fitness[1] *= 1.f / (mmm::abs(r.x) * deltaTime + 1.f);
  //   // fitness[2] *= 1.f / (mmm::abs(r.z) * deltaTime + 1.f);

  //   fitness[0] *=
  //     mmm::product(1.f / (mmm::abs(vec3(r.x, h, r.z)) * deltaTime + 1.f));
  // }

  { // Angles

    float x = 0;
    float i = 0;

    for (auto& part : spider->parts()) {
      if (part.second.hinge != nullptr) {

        float current = part.second.hinge->getHingeAngle();
        x += mmm::abs(current - part.second.restAngle) * deltaTime;
        i += 1;

      } else if (part.second.dof != nullptr) {
        // TODO
      }
    }

    fitness[1] *= 1.f / ((x / i) + 1.f);
  }


  // { // Leg ground contact
  //   float l1 = mmm::clamp(spider->parts()["TarsusL1"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float l2 = mmm::clamp(spider->parts()["TarsusL2"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float l3 = mmm::clamp(spider->parts()["TarsusL3"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float l4 = mmm::clamp(spider->parts()["TarsusL4"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float r1 = mmm::clamp(spider->parts()["TarsusR1"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float r2 = mmm::clamp(spider->parts()["TarsusR2"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float r3 = mmm::clamp(spider->parts()["TarsusR3"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;
  //   float r4 = mmm::clamp(spider->parts()["TarsusR4"].part->rigidBody()->getCenterOfMassPosition().y() - 0.3f, 0.f, 2.f) / 2.f;

  //   float x  = mmm::abs(l1 + l2 + l3 + l4 + r1 + r2 + r3 + r4);

  //   fitness[1] *= 1.f / (mmm::abs(x) + 1.f);
  // }


  // // Calculate the fitness based on the angle of its hinges, dividing the
  // // accumlicated value by the number of hinges
  // fitness[0] += mmm::clamp(maxFitnessAngle / float(i), 0.0, 1.0);

  // // Calculate the fitness based on the position of the sternum. The closer
  // // to 5z the better.
  // fitness[1] += mmm::clamp(1.0 / (mmm::abs(pos - 5) + 1), 0.0, 1.0);

  // // Calculate the fitness based on the height of the sternum. The closer to
  // // 1.5y, the better.
  // fitness[2] += mmm::clamp(1.0 / (mmm::abs(ySternum - 1.0) + 1), 0.0, 1.0);

  // // Calculate the fitness based on the z-velocity of the sternum. The closer
  // // to 1 the better.
  // fitness[3] += mmm::clamp(1.0 / (mmm::abs(zVelocity - 1.0) + 1), 0.0, 1.0);
}

/**
 * @brief
 *   Makes the final calculations of the Phenotype, returning the fitness
 *   as a floating point value
 *
 * @return
 */
float Phenotype::finalizeFitness() {
  // float updates = numUpdates;
  // fitness[0] /= updates;
  // fitness[1] /= updates;
  // fitness[2] /= updates;
  // fitness[3] /= updates;

  // length walked
  // btRigidBody* sternum = spider->parts()["Sternum"].part->rigidBody();
  // auto         pos_    = sternum->getCenterOfMassPosition();
  // mmm::vec3    pos     = mmm::vec3(pos_.x(), pos_.y(), pos_.z());

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
