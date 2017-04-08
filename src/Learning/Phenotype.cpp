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

  bool collided() const {return mHasCollided;}

  void reset() {
    mHasCollided = false;
  }

private:
  bool mHasCollided;
};

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
 *   Checks if the given spider part is either resting against or colliding
 *   against the static terrain.
 *
 *   Returns true if there is a collision
 *
 * @param spiderPart
 *
 * @return
 */
bool Phenotype::collidesWithTerrain(btRigidBody* spiderPart) {
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
bool Phenotype::collidesWithTerrain(Drawable3D* spiderPart) {
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

  auto pi = mmm::constants<float>::pi;

  // construct input vector which is known to be exactly 172 elements
  std::vector<double> inputs;
  inputs.reserve(136);
  inputs.push_back(mmm::sin(duration));
  duration += deltaTime;

  for (auto& part : spider->parts()) {

    vec3  ang = part.second.part->angularVelocity();
    inputs.push_back(ang.x);
    inputs.push_back(ang.y);
    inputs.push_back(ang.z);

    // auto ypos = part.second.part->rigidBody()->getCenterOfMassPosition().y();
    // inputs.push_back(ypos);

    // if (part.second.hinge != nullptr) {

    //   float rot = part.second.hinge->getHingeAngle();
    //   inputs.push_back(rot); // - part.second.restAngle);

    // } else if (part.second.dof != nullptr) {

    //   // TODO
    // }
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
      auto targetAngle =
        (part.second.active)
          ? outputs[i] * 4.f * pi - 2.f * pi + part.second.restAngle
          : part.second.restAngle;

      auto velocity = (targetAngle - currentAngle) * 40.0f;
      part.second.hinge->enableAngularMotor(true, velocity, 4.f);

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
  auto& parts   = spider->parts();
  auto* sternum = parts["Sternum"].part->rigidBody();

  // const std::map<std::string, Spider::Part>& parts = spider->parts();
  //
  // int index = 0;
  // for(const auto& s : Phenotype::FITNESS_HANDLERS) {
  //   s.runFinalize(parts, fitness[index], 1);
  // }

  { // Movement
    if (duration > 2.f && duration < 5.f) {
      auto t = sternum->getCenterOfMassPosition();
      fitness[0] *= score(deltaTime, mmm::clamp(t.y(), 0.f, 0.5f) - 0.5f, 0);


      fitness[8] = mmm::max(fitness[8], t.z() + 1.f);
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
  // if (failed)
  //   return 0.f;

  //const std::map<std::string, Spider::Part>& parts = spider->parts();
  //int index = 0;
  //for(const auto& s : Phenotype::FITNESS_HANDLERS) {
  //  s.runFinalize(parts, fitness[index], 1);
  //}

  // length walked
  // btRigidBody* sternum = spider->parts()["Sternum"].part->rigidBody();
  // auto z = sternum->getCenterOfMassPosition().z();
  fitness[8] /= 100.f;

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

// Below here is where all the fitness handlers are defined

std::vector<Fitness> Phenotype::FITNESS_HANDLERS = {
  Fitness("01",
          R"(
            This is where you can write a longer description of the
            fitness value, documenting it using the new multiline strings
            in C++11
          )",
          [](const std::map<std::string, Spider::Part>&, float, float) -> float{})
};
