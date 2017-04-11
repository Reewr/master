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
class Substrate;

namespace NEAT {
  class Parameters;
  class Population;
}

/**
 * The SpiderSwarm is a class that holds a population of spiders that are used
 * to train a ESHyperNEAT network.
 *
 * \todo Add more documentation
 */
class SpiderSwarm : Logging::Log {
public:
  // This describes the different drawing methods
  //
  // - SpeciesLeaders will draw the best of the species
  // - BestFitness will only draw the best spider
  // - DrawAll will draw all spiders
  //
  enum class DrawingMethod {
    DrawSingleInBatch,
    DrawAllInBatch,
    Species1,
    SpeciesLeaders,
    BestFitness,
    DrawAll
  };

  SpiderSwarm();
  ~SpiderSwarm();

  // Sets the drawing method. Will update on the next update
  void setDrawingMethod(DrawingMethod dm = DrawingMethod::SpeciesLeaders);

  // Returns the current drawing method
  DrawingMethod drawingMethod();

  // Updates the SpiderSwarm which will either run a normal update
  // on the current batch or figure out which batch is next
  void update(float deltaTime);

  // Draws X number of spiders from current batch set by DrawLimit
  void draw(std::shared_ptr<Program>& prog, bool bindTexture);

  // Saves the population, parameters and substrate of spiderswarm
  // to file
  void save(const std::string& filename);

  // Loads the population, parameters and substrate of spiderswarm
  // from file
  void load(const std::string& filename);

  // Returns a reference to the parameter
  NEAT::Parameters& parameters();

  // Restarts the simulation
  void restart();

private:
  std::vector<Phenotype> mPhenotypes;

  size_t mCurrentBatch;
  size_t mBatchStart;
  size_t mBatchEnd;
  size_t mBatchSize;
  size_t mGeneration;

  float mCurrentDuration;
  float mIterationDuration;
  float mBestPossibleFitness;

  bool mDrawDebugNetworks;
  bool mRestartOnNextUpdate;

  // Drawing settings
  DrawingMethod mDrawingMethod;

  std::vector<size_t> mSpeciesLeaders;
  size_t              mBestIndex;

// Save some memory if bullet has profiling on and therefore
// does not allow for threading
#ifdef BT_NO_PROFILE
  std::function<void(std::vector<Phenotype>::iterator begin,
                     std::vector<Phenotype>::iterator end,
                     float                            deltaTime)>
    mWorker;

  std::function<void(std::vector<Phenotype>::iterator begin,
                     std::vector<Phenotype>::iterator end,
                     NEAT::Population&                pop,
                     Substrate&                       sub,
                     NEAT::Parameters&                params)>
    mBuildingESHyperNeatWorker;
#endif

  // If called, it will use as many threads as possible to
  void updateUsingThreads(float deltaTime);

  void updateThreadBatches(float deltaTime);

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
  Substrate*        mSubstrate;
  NEAT::Population* mPopulation;

  void setDefaultParameters();
  void setDefaultSubstrate();
  void setDefaultPopulation();
};
