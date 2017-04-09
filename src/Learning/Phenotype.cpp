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
 *   This structure allows you to check whether two objects
 *   collide with each other. If this detects a collision,
 *   it will set a boolean flag to true that can be retrieved
 *   through the `collided` function.
 */
struct SpiderPartContactResultCallback
  : public btCollisionWorld::ContactResultCallback {
  SpiderPartContactResultCallback() {}

  btScalar addSingleResult(btManifoldPoint&,
                           const btCollisionObjectWrapper*,
                           int,
                           int,
                           const btCollisionObjectWrapper*,
                           int,
                           int) {
    mHasCollided = true;
    return 0;
  }

  bool collided() const { return mHasCollided; }

  void reset() { mHasCollided = false; }

private:
  bool mHasCollided;
};

/**
 * @brief
 *   Returns the Euler angles of a Quaternion
 *
 * @param x
 * @param y
 * @param z
 * @param w
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
 *   Checks if the given spider part is either resting against or colliding
 *   against the static terrain.
 *
 *   Returns true if there is a collision
 *
 * @param spiderPart
 *
 * @return
 */
bool Phenotype::collidesWithTerrain(btRigidBody* spiderPart) const {
  SpiderPartContactResultCallback callback;

  if (world == nullptr || spiderPart == nullptr || planeBody == nullptr)
    return false;

  world->world()->contactPairTest(planeBody, spiderPart, callback);

  return callback.collided();
}

/**
 * @brief
 *   Checks if the given spider part is either resting against or colliding
 *   against the static terrain.
 *
 *   Returns true if there is a collision
 *
 * @param spiderPart
 *
 * @return
 */
bool Phenotype::collidesWithTerrain(Drawable3D* spiderPart) const {
  return collidesWithTerrain(spiderPart->rigidBody());
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

  auto  pi    = mmm::constants<float>::pi;
  auto& parts = spider->parts();

  // construct input vector which is known to be exactly 172 elements
  std::vector<double> inputs;
  inputs.reserve(53);
  inputs.push_back(mmm::sin(duration));
  duration += deltaTime;

  inputs.push_back(collidesWithTerrain(parts["TarsusL1"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusL2"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusL3"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusL4"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusR1"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusR2"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusR3"].part) ? 1.0 : 0.0);
  inputs.push_back(collidesWithTerrain(parts["TarsusR4"].part) ? 1.0 : 0.0);

  for (auto& part : parts) {

    // vec3 ang = part.second.part->angularVelocity();
    // inputs.push_back(ang.x);
    // inputs.push_back(ang.y);
    // inputs.push_back(ang.z);

    // auto v = part.second.part->rigidBody()->getLinearVelocity();
    // inputs.push_back(v.x());
    // inputs.push_back(v.z());
    // inputs.push_back(v.y());


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
  for (auto& part : parts) {
    if (part.second.hinge != nullptr) {

      auto currentAngle = part.second.hinge->getHingeAngle();
      // auto targetAngle = part.second.restAngle;
      auto targetAngle =
        (part.second.active) ?
          outputs[i] * 4.f * pi - 2.f * pi + part.second.restAngle :
          part.second.restAngle;

      auto velocity = mmm::clamp(targetAngle - currentAngle, -0.1f, 0.1f) * 16.f;
      part.second.hinge->enableAngularMotor(true, velocity, 2.f);

      i += (part.second.active) ? 1 : 0;

    } else if (part.second.dof != nullptr) {

      // TODO

      i += (part.second.active) ? 1 : 0;
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
  const std::map<std::string, Spider::Part>& parts = spider->parts();

  int index = 0;
  for(const auto& s : Phenotype::FITNESS_HANDLERS) {
    fitness[index] = s.runCalculation(*this, fitness[index], deltaTime);
    index += 1;
  }

  if (failed)
    mLog->debug("Killed spider");
}

void Phenotype::kill() const {
  if (failed)
    return;

  failed = true;
};

/**
 * @brief
 *   Makes the final calculations of the Phenotype, returning the fitness
 *   as a floating point value
 *
 * @return
 */
float Phenotype::finalizeFitness() {
  const std::map<std::string, Spider::Part>& parts = spider->parts();
  int index = 0;
  for(const auto& s : Phenotype::FITNESS_HANDLERS) {
    fitness[index] = s.runFinalize(*this, fitness[index], 1.f);
    index += 1;
  }

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

  failed          = false;
  duration        = 0;
  numUpdates      = 0;
  fitness         = mmm::vec<9>(1);
  speciesIndex    = speciesId;
  individualIndex = individualId;
}

btStaticPlaneShape* Phenotype::plane =
  new btStaticPlaneShape(btVector3(0, 1, 0), 1);

// Below here is where all the fitness handlers are defined

std::vector<Fitness> Phenotype::FITNESS_HANDLERS = {
  Fitness(
    "Movement (0)",
    "Fitness based on movement is positive z direction.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      auto t = phenotype.spider->parts().at("Sternum").part->rigidBody()->getCenterOfMassPosition();
      return mmm::max(current, t.z() + 1.f);
    },
    [](const auto&, float current, float) -> float {
      return current / 100.f;
    }
  ),
  Fitness(
    "Stability (1)",
    "Fitness based on ground contact.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts = phenotype.spider->parts();

      if (phenotype.collidesWithTerrain(parts.at("Abdomin").part))
        current += deltaTime;

      else if (phenotype.collidesWithTerrain(parts.at("Eye").part))
        current += deltaTime;

      else if (phenotype.collidesWithTerrain(parts.at("Sternum").part))
        current += deltaTime;

      if (current > 4.f)
        phenotype.kill();

      return current;
    },
    [](const auto&, float current, float) -> float {
      return 1.f;
    }
  )
};
