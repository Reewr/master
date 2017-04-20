#pragma once

#include <mmm.hpp>

#include "../Log.hpp"
#include "Fitness.hpp"

struct btDefaultMotionState;
class btRigidBody;
class btStaticPlaneShape;

class World;
class Spider;
class DrawablePhenotype;
class Text3D;

namespace NEAT {
  class NeuralNetwork;
  class Genome;
}

/**
 * A Phenotype represents a single individual in the total population. When
 * the SpiderSwarm is initialized, a set amount of Phenotypes are created,
 * each containing their own Spider, World and Network.
 *
 * The reason for having one world per spider is due to the complexity
 * of the Spider. Even by setting up CollisionGroups and ignoring collisions
 * between spiders, the performance is slower than having one world per
 * spider. The downside of this approach is that it'll require more memory
 */
struct Phenotype : Logging::Log {
  World*               world;
  Spider*              spider;
  NEAT::NeuralNetwork* network;

  btDefaultMotionState*      planeMotion;
  btRigidBody*               planeBody;
  static btStaticPlaneShape* plane;

  DrawablePhenotype* drawablePhenotype;
  Text3D* hoverText;

  mmm::vec<9>  fitness;

  float        variance;
  mutable bool failed;
  float        finalizedFitness;
  float        duration;
  bool         hasFinalized;

  unsigned int genomeId;
  unsigned int speciesIndex;
  unsigned int individualIndex;
  unsigned int numberOfInputs;

  Phenotype();
  ~Phenotype();

  void updateFitness(float deltaTime);
  void kill() const;

  bool hasBeenKilled() const;

  // Returns the final fitness of the Phenotype
  float finalizeFitness();

  // Deletes the memory allocated in reset
  void remove();

  // Checks if a spider part is resting / colliding with the terrain
  bool collidesWithTerrain(btRigidBody* spiderPart) const;
  bool collidesWithTerrain(Drawable3D* spiderPart) const;
  bool collidesWithTerrain(const std::string& name) const;

  // Resets the phenotype back into its original state
  void reset(int          speciesId,
             int          individualId,
             unsigned int id,
             unsigned int numInputs);

  // Performs the update of the phenotype
  void update(float deltaTime);

  void draw(std::shared_ptr<Program>& prog, mmm::vec3 offset, bool bindTexture);

  static std::vector<Fitness> FITNESS_HANDLERS;
};
