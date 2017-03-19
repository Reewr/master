#pragma once

#include <memory>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../Log.hpp"

class Program;
class Spider;
class Terrain;
class World;

namespace NEAT {
  class NeuralNetwork;
  class Parameters;
  class Population;
  class Substrate;
}

/**
 * The SpiderSwarm is a class that holds a population of spiders that are used
 * to train a ESHyperNEAT network.
 *
 * @TODO Add more documentation
 */
class SpiderSwarm : Logging::Log {
public:

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
  struct Phenotype {
    World*               world;
    Spider*              spider;
    NEAT::NeuralNetwork* network;
    mmm::vec<8>          fitness;

    Phenotype();
    ~Phenotype();
  };

  SpiderSwarm();
  ~SpiderSwarm();

  // Updates the SpiderSwarm which will either run a normal update
  // on the current batch or figure out which batch is next
  void update(float deltaTime);

  // Draws X number of spiders from current batch set by DrawLimit
  void draw(std::shared_ptr<Program>& prog, bool bindTexture);

private:

  std::vector<Phenotype> mPhenotypes;

  size_t mCurrentBatch;
  size_t mBatchStart;
  size_t mBatchEnd;
  size_t mBatchSize;

  float  mCurrentDuration;
  float  mIterationDuration;
  size_t mDrawLimit;

  void updateNormal(float deltaTime);
  void updateBatch();
  void updateEpoch();
  void recreatePhenotypes();

  // NEAT stuff
  NEAT::Parameters* mParameters;
  NEAT::Substrate*  mSubstrate;
  NEAT::Population* mPopulation;

  void setDefaultParameters();
  void setDefaultSubstrate();
  void setDefaultPopulation();
};
