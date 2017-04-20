#include "Phenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>
#include <btBulletDynamicsCommon.h>

#include "../3D/Spider.hpp"
#include "../3D/Text3D.hpp"
#include "../3D/World.hpp"
#include "../GlobalLog.hpp"

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

Phenotype::Phenotype()
    : Logging::Log("Phenotype")
    , world(nullptr)
    , spider(nullptr)
    , network(nullptr)
    , planeMotion(nullptr)
    , planeBody(nullptr)
    , drawablePhenotype(nullptr)
    , hoverText(nullptr)
    , fitness(1)
    , variance(0)
    , failed(false)
    , duration(0)
    , hasFinalized(false)
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
float denormalizeHingeAngle(float p, float low, float up, float rest) {
  return p < 0 ? p * mmm::abs(low - rest) + rest
               : p * mmm::abs(up - rest) + rest;
};


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
  // If the robot has been set to a fail state we ignore future simulations.
  if (failed)
    return;

  auto  pi    = mmm::constants<float>::pi;
  auto& parts = spider->parts();

  // Initiate start position
  //
  // We want to the simulation to always be equal for all robots. In order to do
  // this more easily, we move the robot to a resting position before the simulation
  // starts.
  //
  // This makes sure that all positions are equal.
  if (duration < 0.0) {
    for (auto& part : parts) {
      if (part.second.hinge != nullptr) {

        auto currentAngle = part.second.hinge->getHingeAngle();
        float velocity = mmm::clamp(part.second.restAngle - currentAngle, -0.3f, 0.3f) * 16.f;
        part.second.hinge->enableAngularMotor(true, velocity, 2.f);
      } else if (part.second.dof != nullptr) {

        // TODO
      }

    }

    world->doPhysics(deltaTime);

    duration += deltaTime;

    // Return since we dont want the network to be used just yet.
    return;
  }

  duration += deltaTime;

  auto tl1 = parts["TarsusL1"].part->rigidBody()->getCenterOfMassPosition().z();
  auto tl2 = parts["TarsusL2"].part->rigidBody()->getCenterOfMassPosition().z();
  auto tl3 = parts["TarsusL3"].part->rigidBody()->getCenterOfMassPosition().z();
  auto tl4 = parts["TarsusL4"].part->rigidBody()->getCenterOfMassPosition().z();

  auto tr1 = parts["TarsusR1"].part->rigidBody()->getCenterOfMassPosition().z();
  auto tr2 = parts["TarsusR2"].part->rigidBody()->getCenterOfMassPosition().z();
  auto tr3 = parts["TarsusR3"].part->rigidBody()->getCenterOfMassPosition().z();
  auto tr4 = parts["TarsusR4"].part->rigidBody()->getCenterOfMassPosition().z();

  auto neck     = parts["Neck"].part->rigidBody()->getCenterOfMassPosition().z();
  auto abomin   = parts["Abdomin"].part->rigidBody()->getCenterOfMassPosition().z();
  auto& sternum = parts["Sternum"].part->rigidBody()->getCenterOfMassPosition();

  // Initialize the input list, which will contain many many elements.
  std::vector<double> inputs;
  inputs.reserve(numberOfInputs);

  // Add an input that gives some indication of time.
  inputs.push_back(mmm::sin(duration));

  // Give the network an indication of how the legs are positioned
  // relative to some central pieces of the robot. This may be
  // used by the network to know which way to move the elements.
  inputs.push_back(tl1 > neck ? -1.0 : 1.0);
  inputs.push_back(tl2 > sternum.z() ? -1.0 : 1.0);
  inputs.push_back(tl3 > sternum.z() ? -1.0 : 1.0);
  inputs.push_back(tl4 > abomin ? -1.0 : 1.0);

  inputs.push_back(tr1 > neck ? -1.0 : 1.0);
  inputs.push_back(tr2 > sternum.z() ? -1.0 : 1.0);
  inputs.push_back(tr3 > sternum.z() ? -1.0 : 1.0);
  inputs.push_back(tr4 > abomin ? -1.0 : 1.0);

  for (auto& part : parts) {

    // If the part isnt active, ignore it
    if (!part.second.active)
      continue;

    btRigidBody*     body         = part.second.part->rigidBody();
    const btVector3& centerOfMass = body->getCenterOfMassPosition();
    const btVector3& angularVel   = body->getAngularVelocity();
    const btVector3& linearVel    = body->getLinearVelocity();
    btVector3        relative     = centerOfMass - sternum;

    // Add whether or not the element colides with anything.
    inputs.push_back(collidesWithTerrain(body) ? 1.0 : 0.0);

    // Add the height of the element
    inputs.push_back(centerOfMass.y());

    // Add position of the element relative to the Sternum, which
    // represent the 0,0,0 point
    inputs.push_back(relative.x());
    inputs.push_back(relative.y());
    inputs.push_back(relative.z());

    // Add the current angular velocity of the element
    inputs.push_back(angularVel.x());
    inputs.push_back(angularVel.y());
    inputs.push_back(angularVel.z());

    // Add the current linear velocity of the element
    inputs.push_back(linearVel.x());
    inputs.push_back(linearVel.y());
    inputs.push_back(linearVel.z());

    if (part.second.hinge != nullptr) {

      // Add the current rotation of the hinge for the element
      inputs.push_back(part.second.hinge->getHingeAngle());

    } else if (part.second.dof != nullptr) {

      // TODO
    }
  }

  // Throw error if the input is not equal to the expected number of inputs.
  // This is mostly for debugging as we may sometimes forget to add/remove
  // an input when we are adjusting the substrate
  if (inputs.size() != numberOfInputs) {
    mLog->error("Missing inputs. Expected: {}, Got: {}", inputs.size(), numberOfInputs);
    throw std::runtime_error("Phenotype missing inputs. See message above.");
  }

  // Flush the network, resetting its activesum and activations
  // before giving it new input
  network->Flush();
  network->Input(inputs);

  // Activate the network, going through all connections and neurons
  // to set the activesum and activation values. The number of
  // times needed to activate depends on the depth of the network
  for (int a = 0; a < 4; a++) {
    network->Activate();
  }

  std::vector<double> outputs = network->Output();

  // set hinge motor targets based on network output
  size_t i = 0;
  for (auto& part : parts) {
    if (part.second.hinge != nullptr) {

      float currentAngle = part.second.hinge->getHingeAngle();
      float rotation;

      // If the element is active, set the angle to an angle that is mutliplied so
      // its between 2*PI and -2*PI
      //
      // If the element isnt active we wont punish the robot and set the part
      // to a neutral angle that wont be in the way.
      if (part.second.active)
        rotation = 4 * pi * outputs[i] - 2 * pi;
      else
        rotation = mmm::clamp(part.second.restAngle - currentAngle, -0.3f, 0.3f) * 16.0f;

      part.second.hinge->enableAngularMotor(true, rotation, 4.f);

      i += part.second.active ? 1 : 0;

    } else if (part.second.dof != nullptr) {

      // TODO

      i += (part.second.active) ? 1 : 0;
    }
  }

  // Finally, now that all things are set, lets keep updating the
  // physics
  world->doPhysics(deltaTime);

  // After the physics have been executed, evaluate the fitness
  // of the robot.
  updateFitness(deltaTime);
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
void Phenotype::updateFitness(float deltaTime) {
  int index = 0;
  for(const auto& s : Phenotype::FITNESS_HANDLERS) {
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
float Phenotype::finalizeFitness() {
  hasFinalized = true;

  int index = 0;
  for(const auto& s : Phenotype::FITNESS_HANDLERS) {
    fitness[index] = s.runFinalize(*this, fitness[index], 1.f);
    index += 1;
  }

  return fitness[0];
}

/**
 * @brief
 *   Resets the Phenotype by resetting the values
 *   on the world, spider and network. Also resets
 *   the fitness and number of updates.
 *
 *   If all the values are nullptr, allocate them
 */
void Phenotype::reset(int speciesId, int individualId, unsigned int numInputs) {

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
    consInfo.m_friction = 1.0;
    planeBody = new btRigidBody(consInfo);
    world->world()->addRigidBody(planeBody);
  }

  // In order to distinguish the spiders from each other, each spider
  // has an assigned number to it in the format of 'X:Y" where X
  // is the speciesIndex and Y is the individual index. Both of
  // these together gives a unique ID.
  std::string textOverHead = "\\<255,255,255,255:0,0,0,255>" + std::to_string(speciesId) +
                             ":" + std::to_string(individualId) + "\\</>";

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

  hasFinalized    = false;
  failed          = false;
  duration        = -2;
  fitness         = mmm::vec<9>(1);
  speciesIndex    = speciesId;
  individualIndex = individualId;
  numberOfInputs  = numInputs;
}

// In order to save memory, this shape is stored statically on
// the Phenotype and is used by every instance of the Phenotype
btStaticPlaneShape* Phenotype::plane =
  new btStaticPlaneShape(btVector3(0, 1, 0), 1);

// Below here is where all the fitness handlers are defined
std::vector<Fitness> Phenotype::FITNESS_HANDLERS = {

  Fitness(
    "Movement  ",
    "Fitness based on movement is positive z direction.",
    [](const auto& phenotype, float current, float) -> float {
      /* const auto& parts = phenotype.spider->parts(); */
      /* auto t = parts.at("Sternum").part->rigidBody()->getCenterOfMassPosition(); */
      /* return mmm::max(current, t.z() + 1.f); */
      return 1.f;
    },
    [](const Phenotype& p, float, float) -> float {
      float z = p.spider->parts().at("Sternum").part->rigidBody()->getCenterOfMassPosition().z();

      return z;
    }
  ),

  Fitness(
    "Collision  ",
    "Fitness based on height with Sternum",
    [](const Phenotype& phenotype, float current, float dt) -> float {
      const auto& parts = phenotype.spider->parts();

      auto body = parts.at("Sternum").part->rigidBody();
      auto h    = body->getCenterOfMassPosition().y();
      auto q    = body->getOrientation();
      auto v    = getEulerAngles(q.x(), q.y(), q.z(), q.w());

      float hasTooSteepAngles = 1;
      hasTooSteepAngles *= score(dt, v.x, mmm::radians(35.f));
      hasTooSteepAngles *= score(dt, v.z, mmm::radians(35.f));
      hasTooSteepAngles *= score(dt, v.y + mmm::radians(90.f), mmm::radians(50.f));

      // If the angle of sternum is too far away from stable
      if (hasTooSteepAngles < 1.0) {
        /* phenotype.kill(); */
        return current;
      }

      // If the height is too low or too high
      if (h > 1.9) {
        /* debug("Killed {}-{} due to height", phenotype.speciesIndex, phenotype.individualIndex); */
        /* phenotype.kill(); */
        return current;
      }

      // If any of the important pieces are hitting the floor
      if (phenotype.collidesWithTerrain(body) ||
          phenotype.collidesWithTerrain("Eye") ||
          phenotype.collidesWithTerrain("Neck")) {
        /* debug("Killed {}-{} due to Eye/Neck/Sternum", phenotype.speciesIndex, phenotype.individualIndex); */
        /* phenotype.kill(); */
        return current;
      }

      // If even more parts of its legs are hitting the floor
      if (phenotype.collidesWithTerrain("PatellaL1") ||
          phenotype.collidesWithTerrain("PatellaL2") ||
          phenotype.collidesWithTerrain("PatellaL3") ||
          phenotype.collidesWithTerrain("PatellaL4") ||
          phenotype.collidesWithTerrain("PatellaR1") ||
          phenotype.collidesWithTerrain("PatellaR2") ||
          phenotype.collidesWithTerrain("PatellaR3") ||
          phenotype.collidesWithTerrain("PatellaR4")) {
        /* debug("Killed {}-{} due to patella", phenotype.speciesIndex, phenotype.individualIndex); */
        /* phenotype.kill(); */
        return current;
      }

      // Lastly, it shouldnt rest on the bendy bit by the sternum
      if (phenotype.collidesWithTerrain("FemurL1") ||
          phenotype.collidesWithTerrain("FemurL2") ||
          phenotype.collidesWithTerrain("FemurL3") ||
          phenotype.collidesWithTerrain("FemurL4") ||
          phenotype.collidesWithTerrain("FemurR1") ||
          phenotype.collidesWithTerrain("FemurR2") ||
          phenotype.collidesWithTerrain("FemurR3") ||
          phenotype.collidesWithTerrain("FemurR4")) {
        /* debug("Killed {}-{} due to femur", phenotype.speciesIndex, phenotype.individualIndex); */
        /* phenotype.kill(); */
        return current;
      }

      // If it passed all these tests, yay for it.
      return current + dt;
    },
    [](const Phenotype&, float current, float) -> float {
      return current - 1;
    }
  )
};
