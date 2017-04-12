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
    , hoverText(nullptr)
    , fitness(1)
    , numUpdates(0)
    , variance(0)
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
  if (failed)
    return;

  auto  pi    = mmm::constants<float>::pi;
  auto& parts = spider->parts();

  // construct input vector which is known to be exactly 172 elements
  std::vector<double> inputs;
  inputs.reserve(188);

  inputs.push_back(mmm::sin(duration));

  // float phase0 = mmm::sin(duration);
  // float phase1 = mmm::sin(duration + pi);

  // inputs.push_back(phase0);
  // inputs.push_back(phase1);
  // inputs.push_back(phase0);
  // inputs.push_back(phase1);
  // inputs.push_back(phase1);
  // inputs.push_back(phase0);
  // inputs.push_back(phase1);
  // inputs.push_back(phase0);

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

    auto v = part.second.part->rigidBody()->getLinearVelocity();
    inputs.push_back(v.x());
    inputs.push_back(v.z());
    inputs.push_back(v.y());


    if (part.second.hinge != nullptr) {

      float rot = part.second.hinge->getHingeAngle();

      if (part.second.hinge->hasLimit()) {
        float low = part.second.hinge->getLowerLimit();
        float up  = part.second.hinge->getUpperLimit();
        rot = normalizeHingeAngle(rot, low, up, 0.f);
      } else {
        rot = normalizeHingeAngle(rot, -pi, pi, 0.f);
      }

      inputs.push_back(rot);

    } else if (part.second.dof != nullptr) {

      // TODO
    }
  }

  // activate network to retrieve output vector
  network->Flush();
  network->Input(inputs);

  for (unsigned int i = 0; i < 3; ++i)
    network->Activate();

  std::vector<double> outputs = network->Output();

  // set hinge motor targets based on network output
  size_t i = 0;
  for (auto& part : parts) {
    if (part.second.hinge != nullptr) {

      auto currentAngle = part.second.hinge->getHingeAngle();
      // auto targetAngle = part.second.restAngle;
      // auto targetAngle =
      //   (part.second.active) ?
      //     outputs[i] * 4.f * pi - 2.f * pi + part.second.restAngle :
      //     part.second.restAngle;

      float targetAngle;
      float rot = part.second.active ? outputs[i] : 0.f;

      if (part.second.hinge->hasLimit()) {
        float low = part.second.hinge->getLowerLimit();
        float up  = part.second.hinge->getUpperLimit();
        targetAngle = denormalizeHingeAngle(rot, low, up, 0.f);
      } else {
        targetAngle = denormalizeHingeAngle(rot, -pi, pi, 0.f);
      }

      float velocity = mmm::clamp(targetAngle - currentAngle, -0.1f, 0.1f) * 16.f;
      part.second.hinge->enableAngularMotor(true, velocity, 2.f);

      i += (part.second.active) ? 1 : 0;

    } else if (part.second.dof != nullptr) {

      // TODO

      i += (part.second.active) ? 1 : 0;
    }
  }

  // update physics
  world->doPhysics(deltaTime);

  // hack fitness
  float prev = variance;
  float next = 0.f;
  for (auto x : outputs)
    next += mmm::abs(x - 0.5);
  fitness[0] = next == prev ? fitness[0] - deltaTime * 0.5 : 1.f;
  variance = next;

  // judge fitness, if we can
  updateFitness(deltaTime);

  // update counter
  ++numUpdates;
}

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

  if (failed)
    mLog->debug("Killed spider");
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

  std::string textOverHead = "\\<255,255,255,255:0,0,0,255>" + std::to_string(speciesId) +
                             ":" + std::to_string(individualId) + "\\</>";

  if (hoverText == nullptr) {
    hoverText = new Text3D("Font::Dejavu", textOverHead, mmm::vec3(0, 0, 0));
  } else {
    hoverText->setText(textOverHead);
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
    "Laziness  ",
    "Fitness based on variance of network outputs.",
    [](const auto& phenotype, float current, float) -> float {

      // This is a hack, as the actual variance is calculated within the
      // `update` function... This is also why this fitness absolutely has to be
      // the first fitness value in the handler array. Should fix this as some-
      // point...

      if (current <= 0.f) {
        phenotype.kill();
        return -1.f;
      }

      return current;
    },
    [](const auto&, float current, float) -> float {
      if (current <= 0.f)
        return 0.f;
      return 1.f;
    }
  ),

  Fitness(
    "Movement  ",
    "Fitness based on movement is positive z direction.",
    [](const auto& phenotype, float current, float) -> float {
      const auto& parts = phenotype.spider->parts();
      auto t = parts.at("Sternum").part->rigidBody()->getCenterOfMassPosition();
      return mmm::max(current, t.z() + 1.f);
    },
    [](const auto&, float current, float) -> float {
      return (current - 1.f) / 100.f;
    }
  ),
  Fitness(
    "Height    ",
    "Fitness based on sternum height.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts   = phenotype.spider->parts();
      auto*       sternum = parts.at("Sternum").part->rigidBody();
      auto        t       = sternum->getCenterOfMassPosition();

      return current * score(deltaTime, t.y() - 0.8f, 0.2f);
    }
  ),
  Fitness(
    "Stability ",
    "Fitness based on sternum rotation.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts = phenotype.spider->parts();

      auto* sternum = parts.at("Sternum").part->rigidBody();
      auto  q       = sternum->getOrientation();
      vec3  r       = getEulerAngles(q.x(), q.y(), q.z(), q.w());
      r.y += mmm::radians(90.f);

      current *= score(deltaTime, r.x, mmm::radians(5.f));
      current *= score(deltaTime, r.y, mmm::radians(10.f));
      current *= score(deltaTime, r.z, mmm::radians(5.f));

      return current;
    }
  ),

  Fitness(
    "Trochanter",
    "Fitness based on asymmetry of the trochanter joints.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts = phenotype.spider->parts();

      auto l1 = parts.at("TrochanterL1");
      auto l2 = parts.at("TrochanterL2");
      auto l3 = parts.at("TrochanterL3");
      auto l4 = parts.at("TrochanterL4");
      auto r1 = parts.at("TrochanterR1");
      auto r2 = parts.at("TrochanterR2");
      auto r3 = parts.at("TrochanterR3");
      auto r4 = parts.at("TrochanterR4");

      float a1 = normalizeHingeAngle(l1.hinge->getHingeAngle(), l1.hinge->getLowerLimit(), l1.hinge->getUpperLimit(), l1.restAngle);
      float a2 = normalizeHingeAngle(l2.hinge->getHingeAngle(), l2.hinge->getLowerLimit(), l2.hinge->getUpperLimit(), l2.restAngle);
      float a3 = normalizeHingeAngle(l3.hinge->getHingeAngle(), l3.hinge->getLowerLimit(), l3.hinge->getUpperLimit(), l3.restAngle);
      float a4 = normalizeHingeAngle(l4.hinge->getHingeAngle(), l4.hinge->getLowerLimit(), l4.hinge->getUpperLimit(), l4.restAngle);
      float b1 = normalizeHingeAngle(r1.hinge->getHingeAngle(), r1.hinge->getLowerLimit(), r1.hinge->getUpperLimit(), r1.restAngle);
      float b2 = normalizeHingeAngle(r2.hinge->getHingeAngle(), r2.hinge->getLowerLimit(), r2.hinge->getUpperLimit(), r2.restAngle);
      float b3 = normalizeHingeAngle(r3.hinge->getHingeAngle(), r3.hinge->getLowerLimit(), r3.hinge->getUpperLimit(), r3.restAngle);
      float b4 = normalizeHingeAngle(r4.hinge->getHingeAngle(), r4.hinge->getLowerLimit(), r4.hinge->getUpperLimit(), r4.restAngle);

      // Asymmetry between opposite legs
      current *= score(deltaTime, a1 + b1, 0.05f);
      current *= score(deltaTime, a2 + b2, 0.05f);
      current *= score(deltaTime, a3 + b3, 0.05f);
      current *= score(deltaTime, a4 + b4, 0.05f);

      // Symmetry between counter opposite legs
      current *= score(deltaTime, a1 - b2, 0.05f);
      current *= score(deltaTime, a2 - b3, 0.05f);
      current *= score(deltaTime, a3 - b4, 0.05f);
      current *= score(deltaTime, a4 - b1, 0.05f);

      return current;
    }
  ),
  Fitness(
    "Swing     ",
    "Fitness based on the movement of the trochanter joints.",
    [](const auto& phenotype, float current, float) -> float {
      const auto& parts = phenotype.spider->parts();

      auto l1 = parts.at("TrochanterL1");
      auto l2 = parts.at("TrochanterL2");
      auto l3 = parts.at("TrochanterL3");
      auto l4 = parts.at("TrochanterL4");
      auto r1 = parts.at("TrochanterR1");
      auto r2 = parts.at("TrochanterR2");
      auto r3 = parts.at("TrochanterR3");
      auto r4 = parts.at("TrochanterR4");

      float a1 = normalizeHingeAngle(l1.hinge->getHingeAngle(), l1.hinge->getLowerLimit(), l1.hinge->getUpperLimit(), l1.restAngle);
      float a2 = normalizeHingeAngle(l2.hinge->getHingeAngle(), l2.hinge->getLowerLimit(), l2.hinge->getUpperLimit(), l2.restAngle);
      float a3 = normalizeHingeAngle(l3.hinge->getHingeAngle(), l3.hinge->getLowerLimit(), l3.hinge->getUpperLimit(), l3.restAngle);
      float a4 = normalizeHingeAngle(l4.hinge->getHingeAngle(), l4.hinge->getLowerLimit(), l4.hinge->getUpperLimit(), l4.restAngle);
      float b1 = normalizeHingeAngle(r1.hinge->getHingeAngle(), r1.hinge->getLowerLimit(), r1.hinge->getUpperLimit(), r1.restAngle);
      float b2 = normalizeHingeAngle(r2.hinge->getHingeAngle(), r2.hinge->getLowerLimit(), r2.hinge->getUpperLimit(), r2.restAngle);
      float b3 = normalizeHingeAngle(r3.hinge->getHingeAngle(), r3.hinge->getLowerLimit(), r3.hinge->getUpperLimit(), r3.restAngle);
      float b4 = normalizeHingeAngle(r4.hinge->getHingeAngle(), r4.hinge->getLowerLimit(), r4.hinge->getUpperLimit(), r4.restAngle);

      float x = 1.f;

      x = mmm::min(x, a1);
      x = mmm::min(x, a2);
      x = mmm::min(x, a3);
      x = mmm::min(x, a4);
      x = mmm::min(x, b1);
      x = mmm::min(x, b2);
      x = mmm::min(x, b3);
      x = mmm::min(x, b4);

      x += 1.f;

      return mmm::max(current, x);
    },
    [](const auto&, float current, float) -> float {
      return current - 1.f;
    }
  ),

  Fitness(
    "Femur     ",
    "Fitness based on symmetry or asymmetry of the femur joints.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts = phenotype.spider->parts();

      auto l1 = parts.at("FemurL1");
      auto l2 = parts.at("FemurL2");
      auto l3 = parts.at("FemurL3");
      auto l4 = parts.at("FemurL4");
      auto r1 = parts.at("FemurR1");
      auto r2 = parts.at("FemurR2");
      auto r3 = parts.at("FemurR3");
      auto r4 = parts.at("FemurR4");

      float a1 = normalizeHingeAngle(l1.hinge->getHingeAngle(), l1.hinge->getLowerLimit(), l1.hinge->getUpperLimit(), l1.restAngle);
      float a2 = normalizeHingeAngle(l2.hinge->getHingeAngle(), l2.hinge->getLowerLimit(), l2.hinge->getUpperLimit(), l2.restAngle);
      float a3 = normalizeHingeAngle(l3.hinge->getHingeAngle(), l3.hinge->getLowerLimit(), l3.hinge->getUpperLimit(), l3.restAngle);
      float a4 = normalizeHingeAngle(l4.hinge->getHingeAngle(), l4.hinge->getLowerLimit(), l4.hinge->getUpperLimit(), l4.restAngle);
      float b1 = normalizeHingeAngle(r1.hinge->getHingeAngle(), r1.hinge->getLowerLimit(), r1.hinge->getUpperLimit(), r1.restAngle);
      float b2 = normalizeHingeAngle(r2.hinge->getHingeAngle(), r2.hinge->getLowerLimit(), r2.hinge->getUpperLimit(), r2.restAngle);
      float b3 = normalizeHingeAngle(r3.hinge->getHingeAngle(), r3.hinge->getLowerLimit(), r3.hinge->getUpperLimit(), r3.restAngle);
      float b4 = normalizeHingeAngle(r4.hinge->getHingeAngle(), r4.hinge->getLowerLimit(), r4.hinge->getUpperLimit(), r4.restAngle);

      current *= score(deltaTime, mmm::min(a1 + b1, a1 - b1), 0.05f);
      current *= score(deltaTime, mmm::min(a2 + b2, a2 - b2), 0.05f);
      current *= score(deltaTime, mmm::min(a3 + b3, a3 - b3), 0.05f);
      current *= score(deltaTime, mmm::min(a4 + b4, a4 - b4), 0.05f);

      return current;
    }
  ),
  Fitness(
    "Patella   ",
    "Fitness based on symmetry or asymmetry of the patella joints.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts = phenotype.spider->parts();

      auto l1 = parts.at("PatellaL1");
      auto l2 = parts.at("PatellaL2");
      auto l3 = parts.at("PatellaL3");
      auto l4 = parts.at("PatellaL4");
      auto r1 = parts.at("PatellaR1");
      auto r2 = parts.at("PatellaR2");
      auto r3 = parts.at("PatellaR3");
      auto r4 = parts.at("PatellaR4");

      float a1 = normalizeHingeAngle(l1.hinge->getHingeAngle(), l1.hinge->getLowerLimit(), l1.hinge->getUpperLimit(), l1.restAngle);
      float a2 = normalizeHingeAngle(l2.hinge->getHingeAngle(), l2.hinge->getLowerLimit(), l2.hinge->getUpperLimit(), l2.restAngle);
      float a3 = normalizeHingeAngle(l3.hinge->getHingeAngle(), l3.hinge->getLowerLimit(), l3.hinge->getUpperLimit(), l3.restAngle);
      float a4 = normalizeHingeAngle(l4.hinge->getHingeAngle(), l4.hinge->getLowerLimit(), l4.hinge->getUpperLimit(), l4.restAngle);
      float b1 = normalizeHingeAngle(r1.hinge->getHingeAngle(), r1.hinge->getLowerLimit(), r1.hinge->getUpperLimit(), r1.restAngle);
      float b2 = normalizeHingeAngle(r2.hinge->getHingeAngle(), r2.hinge->getLowerLimit(), r2.hinge->getUpperLimit(), r2.restAngle);
      float b3 = normalizeHingeAngle(r3.hinge->getHingeAngle(), r3.hinge->getLowerLimit(), r3.hinge->getUpperLimit(), r3.restAngle);
      float b4 = normalizeHingeAngle(r4.hinge->getHingeAngle(), r4.hinge->getLowerLimit(), r4.hinge->getUpperLimit(), r4.restAngle);

      current *= score(deltaTime, mmm::min(a1 + b1, a1 - b1), 0.05f);
      current *= score(deltaTime, mmm::min(a2 + b2, a2 - b2), 0.05f);
      current *= score(deltaTime, mmm::min(a3 + b3, a3 - b3), 0.05f);
      current *= score(deltaTime, mmm::min(a4 + b4, a4 - b4), 0.05f);

      return current;
    }
  ),
  Fitness(
    "Tibia     ",
    "Fitness based on symmetry or asymmetry of the tibia joints.",
    [](const auto& phenotype, float current, float deltaTime) -> float {
      const auto& parts = phenotype.spider->parts();

      auto l1 = parts.at("TibiaL1");
      auto l2 = parts.at("TibiaL2");
      auto l3 = parts.at("TibiaL3");
      auto l4 = parts.at("TibiaL4");
      auto r1 = parts.at("TibiaR1");
      auto r2 = parts.at("TibiaR2");
      auto r3 = parts.at("TibiaR3");
      auto r4 = parts.at("TibiaR4");

      float a1 = normalizeHingeAngle(l1.hinge->getHingeAngle(), l1.hinge->getLowerLimit(), l1.hinge->getUpperLimit(), l1.restAngle);
      float a2 = normalizeHingeAngle(l2.hinge->getHingeAngle(), l2.hinge->getLowerLimit(), l2.hinge->getUpperLimit(), l2.restAngle);
      float a3 = normalizeHingeAngle(l3.hinge->getHingeAngle(), l3.hinge->getLowerLimit(), l3.hinge->getUpperLimit(), l3.restAngle);
      float a4 = normalizeHingeAngle(l4.hinge->getHingeAngle(), l4.hinge->getLowerLimit(), l4.hinge->getUpperLimit(), l4.restAngle);
      float b1 = normalizeHingeAngle(r1.hinge->getHingeAngle(), r1.hinge->getLowerLimit(), r1.hinge->getUpperLimit(), r1.restAngle);
      float b2 = normalizeHingeAngle(r2.hinge->getHingeAngle(), r2.hinge->getLowerLimit(), r2.hinge->getUpperLimit(), r2.restAngle);
      float b3 = normalizeHingeAngle(r3.hinge->getHingeAngle(), r3.hinge->getLowerLimit(), r3.hinge->getUpperLimit(), r3.restAngle);
      float b4 = normalizeHingeAngle(r4.hinge->getHingeAngle(), r4.hinge->getLowerLimit(), r4.hinge->getUpperLimit(), r4.restAngle);

      current *= score(deltaTime, mmm::min(a1 + b1, a1 - b1), 0.05f);
      current *= score(deltaTime, mmm::min(a2 + b2, a2 - b2), 0.05f);
      current *= score(deltaTime, mmm::min(a3 + b3, a3 - b3), 0.05f);
      current *= score(deltaTime, mmm::min(a4 + b4, a4 - b4), 0.05f);

      return current;
    }
  )
};
