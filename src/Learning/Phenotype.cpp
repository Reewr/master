#include "Phenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>
#include <btBulletDynamicsCommon.h>

#include "../3D/Spider.hpp"
#include "../3D/Text3D.hpp"
#include "../3D/World.hpp"
#include "../GlobalLog.hpp"

#include "DrawablePhenotype.hpp"
#include "Fitness.hpp"
#include "Substrate.hpp"
#include "../Experiments/Experiment.hpp"

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
  bool mHasCollided = false;
};

/**
 * @brief
 *   Calculates a score based on on the `zeroIsBest` value.
 *   deltaTime is multiplied with it to make sure that it doesnt
 *   affect the rest of the simulations too much.
 *
 * @param deltaTime
 * @param zeroIsBest
 * @param bias
 *
 * @return
 */
float score(float deltaTime, float zeroIsBest, float bias = 0.05f) {
  return 1.f / (mmm::max(mmm::abs(zeroIsBest) - bias, 0.f) * deltaTime + 1.f);
};


/**
 * @brief
 *   Returns the Euler angles of a Quaternion, as in the rotation
 *   around a specific axis.
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

mmm::vec3 fromVector(const btVector3& v) {
  return vec3(v.x(), v.y(), v.z());
}


/**
 * @brief
 *   Normalizes the hinge angle between -1 and 1 depending
 *   on what `low` and `up`.
 *
 * @param angle
 * @param low
 * @param up
 * @param rest
 *
 * @return
 */
float normalizeHingeAngle(float angle, float low, float up, float rest) {
  if (angle < low)
    angle += 2.f * mmm::constants<float>::pi;
  if (angle > up)
    angle -= 2.f * mmm::constants<float>::pi;

  if (angle - rest == 0.f)
    return 0.f;

  return angle < rest ? -(angle - rest) / (low - rest)
                      : (angle - rest) / (up - rest);
};

/**
 * @brief
 *   Does the opposite of normalizeHingeAngle given the same
 *   `low`, `up` and `rest` arguments.
 *
 * @param p
 * @param low
 * @param up
 * @param rest
 *
 * @return
 */
float denormalizeHingeAngle(float p, float low, float up, float rest) {
  return p < 0 ? p * mmm::abs(low - rest) + rest
               : p * mmm::abs(up - rest) + rest;
};


Phenotype::Phenotype()
    : Logging::Log("Phenotype")
    , world(nullptr)
    , spider(nullptr)
    , network(nullptr)
    , planeMotion(nullptr)
    , planeBody(nullptr)
    , drawablePhenotype(nullptr)
    , hoverText(nullptr)
    , fitness(0)
    , failed(false)
    , finalizedFitness(0)
    , duration(0)
    , hasFinalized(false)
    , genomeId(0)
    , speciesIndex(0)
    , individualIndex(0) {}

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
  delete hoverText;
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
 *   Checks if the given spider part is either resting against or colliding
 *   against the static terrain.
 *
 *   Just like the other collides with terrain but allows the use
 *   of the name.
 *
 *   Returns true if there is a collision
 *
 * @param spiderPart
 *
 * @return
 */
bool Phenotype::collidesWithTerrain(const std::string& str) const {
  if (spider == nullptr)
    return false;

  const auto& parts = spider->parts();

  if (parts.count(str))
    return collidesWithTerrain(parts.at(str).part->rigidBody());

  return false;
}

/**
 * @brief
 *   There may be times when we want to the Phenotype to be in specific
 *   positions before the simulation takes place.
 *
 *   This function puts the robot into a standing/resting position.
 *
 * @param deltaTime
 */
void Phenotype::updatePrepareStanding(float deltaTime) {
  // Initiate start position
  //
  // We want to the simulation to always be equal for all robots. In order to do
  // this more easily, we move the robot to a resting position before the simulation
  // starts.
  //
  // This makes sure that all positions are equal.
  for (auto& part : spider->parts()) {
    if (part.second.hinge != nullptr) {

      float currentAngle = part.second.hinge->getHingeAngle();
      float velocity =
        mmm::clamp(part.second.restAngle - currentAngle, -1.f, 1.f) * 16.f;
      part.second.hinge->enableAngularMotor(true, velocity, 5.f);
    } else if (part.second.dof != nullptr) {

      // TODO
    }
  }

  world->doPhysics(deltaTime);

  duration += deltaTime;
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
void Phenotype::update(const Experiment& experiment) {
  // If the robot has been set to a fail state we ignore future simulations.
  if (failed)
    return;

  float deltaTime = experiment.parameters().deltaTime;
  // If the duration is less than 0, prepare the robot
  // to be standing
  if (duration < 0.0)
    return updatePrepareStanding(deltaTime);

  auto  pi    = mmm::constants<float>::pi;
  auto& parts = spider->parts();

  duration += deltaTime;

  std::vector<double> inputs = experiment.inputs(*this);

  // Throw error if the input is not equal to the expected number of inputs.
  // This is mostly for debugging as we may sometimes forget to add/remove
  // an input when we are adjusting the substrate
  if (inputs.size() != experiment.numInputs()) {
    mLog->error("Missing inputs. Expected: {}, Got: {}", inputs.size(), experiment.numInputs());
    throw std::runtime_error("Phenotype missing inputs. See message above.");
  }

  // Flush the network, resetting its activesum and activations
  // before giving it new input
  network->Flush();
  network->Input(inputs);

  // Activate the network, going through all connections and neurons
  // to set the activesum and activation values. The number of
  // times needed to activate depends on the depth of the network
  for (int a = 0; a < experiment.parameters().numActivates; a++) {
    if (experiment.substrate()->m_leaky)
      network->ActivateLeaky(deltaTime);
    else
      network->Activate();
  }

  experiment.outputs(*this, network->Output());

  // Finally, now that all things are set, lets keep updating the
  // physics
  world->doPhysics(deltaTime);

  // After the physics have been executed, evaluate the fitness
  // of the robot.
  updateFitness(experiment);
}

/**
 * @brief
 *   Draw the Phenotype by drawing the spider with an offset as
 *   well as text above it to make it easier to distinguish the
 *   different robots
 *
 * @param prog
 * @param offset
 * @param bindTexture
 */
void Phenotype::draw(std::shared_ptr<Program>& prog,
                     mmm::vec3                 offset,
                     bool                      bindTexture) {

  if (spider == nullptr)
    return;

  spider->enableUpdatingFromPhysics();
  spider->draw(prog, offset, bindTexture);

  if (bindTexture && hoverText != nullptr) {
    auto& pos = spider->parts().at("Sternum").part->position();
    hoverText->draw(pos + mmm::vec3(0, 3, 0) + offset);
  }
}

/**
 * @brief
 *   Runs through the fitness calculations and executes
 *   each and everyone of them.
 *
 * @param deltaTime
 */
void Phenotype::updateFitness(const Experiment& experiment) {
  int index = 0;
  float deltaTime = experiment.parameters().deltaTime;
  for(const auto& s : experiment.fitnessFunctions()) {
    fitness[index] = s.runCalculation(*this, fitness[index], deltaTime);
    index += 1;
  }
}

/**
 * @brief
 *   If the spider does something that we recognize as something
 *   that should never be done, we can use this function to stop
 *   all further simulation.
 */
void Phenotype::kill() const {
  if (failed)
    return;

  failed = true;
};

/**
 * @brief
 *   Returns whether or not the spider has been killed due to
 *   incompetence.
 *
 * @return
 */
bool Phenotype::hasBeenKilled() const {
  return failed;
}

/**
 * @brief
 *   Makes the final calculations of the Phenotype, returning the fitness
 *   as a floating point value
 *
 * @return
 */
float Phenotype::finalizeFitness(const Experiment& experiment) {
  hasFinalized = true;

  int index = 0;
  for(const auto& s : experiment.fitnessFunctions()) {
    fitness[index] = s.runFinalize(*this, fitness[index], duration);
    index += 1;
  }

  finalizedFitness = mmm::sum(fitness);
  // finalizedFitness = score(duration, mmm::sum(mmm::drop<1>(fitness)), 0.f) * fitness[0];

  return finalizedFitness;
}

/**
 * @brief
 *   Resets the Phenotype by resetting the values
 *   on the world, spider and network. Also resets
 *   the fitness and number of updates.
 *
 *   If all the values are nullptr, allocate them
 *
 *   This function is used instead of deallocating and reallocating
 *   the elements for each loop. This allows us to reuse the Phenotypes,
 *   improving the performance.
 */
void Phenotype::reset(int          speciesId,
                      int          speciesIndex,
                      int          individualIndex,
                      unsigned int genomeId) {

  // Create the world or reset it if it exists
  if (world == nullptr)
    world = new World(mmm::vec3(0, -9.81, 0));
  else
    world->reset();

  // Initiate the DrawablePhenotype class that lets
  // you draw the network that the Phenotype represents
  if (drawablePhenotype == nullptr)
    drawablePhenotype = new DrawablePhenotype();

  // Create the plane that the spider will walk upon
  if (planeBody == nullptr) {
    planeMotion = new btDefaultMotionState(
      btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo consInfo(0,
                                                      planeMotion,
                                                      plane,
                                                      btVector3(0, 0, 0));
    consInfo.m_friction = 0.84;
    planeBody = new btRigidBody(consInfo);
    world->world()->addRigidBody(planeBody);
  }

  // In order to distinguish the spiders from each other, each spider
  // has an assigned number to it in the format of 'X:Y" where X
  // is the speciesIndex and Y is the individual index. Both of
  // these together gives a unique ID.
  std::string textOverHead = "\\<255,255,255,255:0,0,0,255>" +
                             std::to_string(speciesId) + " - " +
                             std::to_string(speciesIndex) + ":" +
                             std::to_string(individualIndex) + "\\</>";

  if (hoverText == nullptr) {
    hoverText = new Text3D("Font::Dejavu", textOverHead, mmm::vec3(0, 0, 0));
  } else {
    hoverText->setText(textOverHead);
  }

  // Create the spider and add it to the world if it doesnt exist
  // otherwise reset it to start position
  if (spider == nullptr) {
    spider = new Spider();
    world->addObject(spider);
    world->enablePhysics();
  } else {
    spider->reset();
  }

  // Reset the network
  if (network == nullptr)
    network = new NEAT::NeuralNetwork();
  else {
    network->Clear();
    network->Flush();
  }

  hasFinalized     = false;
  failed           = false;
  finalizedFitness = 0;
  duration         = -1;
  fitness          = mmm::vec<9>(0);

  this->speciesId       = speciesId;
  this->speciesIndex    = speciesIndex;
  this->individualIndex = individualIndex;
  this->genomeId        = genomeId;
}

// In order to save memory, this shape is stored statically on
// the Phenotype and is used by every instance of the Phenotype
btStaticPlaneShape* Phenotype::plane =
  new btStaticPlaneShape(btVector3(0, 1, 0), 1);
