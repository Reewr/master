#pragma once

#include <mmm.hpp>
#include <vector>

#include "../Log.hpp"

struct btDefaultMotionState;
class btRigidBody;
class btStaticPlaneShape;

class World;
class Spider;
class DrawablePhenotype;
class Text3D;
class Experiment;
class Drawable3D;
class Program;

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

  DrawablePhenotype* drawablePhenotype;
  Text3D* hoverText;

  mmm::vec<9>  fitness;
  mmm::vec3 initialPosition;

  // std::vector<std::vector<float>> tmp;

  mutable bool failed;
  float        finalizedFitness;
  float        duration;
  bool         hasFinalized;

  unsigned int genomeId;
  unsigned int speciesId;
  unsigned int speciesIndex;
  unsigned int individualIndex;

  Phenotype();
  ~Phenotype();

  // Returns whether or not the robot has been killed
  bool hasBeenKilled() const;

  // Returns the final fitness of the Phenotype
  float finalizeFitness(const Experiment& experiment);

  // Deletes the memory allocated in reset
  void remove();

  // Checks if a spider part is resting / colliding with the terrain
  bool collidesWithTerrain(btRigidBody* spiderPart) const;
  bool collidesWithTerrain(Drawable3D* spiderPart) const;
  bool collidesWithTerrain(const std::string& name) const;

  // Resets the phenotype back into its original state
  void reset(int          speciesId,
             int          speciesIndex,
             int          individualIndex,
             unsigned int genomeId);

  // Performs the update of the phenotype
  void update(const Experiment& experiment);

  // Draws the spider representing the phenotype together with its text
  void draw(std::shared_ptr<Program>& prog, mmm::vec3 offset, bool bindTexture);

  static btStaticPlaneShape* plane;

  // Kills the spider, stopping the evaluation of it
  void kill() const;

private:

  // Prepares the phenotype for simulation
  void updatePrepareStanding(float deltatime);

  // Updates the fitness of the phenotype by
  // running the fitness handlers
  void updateFitness(const Experiment& experiment);
};
