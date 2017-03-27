#pragma once

#include <memory>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../Log.hpp"
#include "Phenotype.hpp"

class Program;
class Spider;
class Terrain;
class World;

namespace NEAT {
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

  // Goes through the current batch and updates each spider in
  // current batch with physics and neural network activation
  void updateNormal(float deltaTime);

  // Increments the batch
  void setNextBatch();

  // When all batches are complete, update the epoch,
  // resetting all spiders
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
