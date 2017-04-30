#pragma once

#include <memory>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../Log.hpp"
#include "Phenotype.hpp"
#include "Statistics.hpp"

#include <Genome.h>

class Program;
class Spider;
class Terrain;
class World;
class Substrate;

namespace NEAT {
  class Parameters;
  class Population;
  class Substrate;
}

/**
 * The SpiderSwarm is a class that holds a population of spiders that are used
 * to train a ESHyperNEAT network.
 *
 * This class handles all of the simulation through the use of Phenotypes,
 * it also handles building Neural Networks.
 *
 * In addition to this, it handles drawing of the Phenotypes in different
 * ways described by the DrawingMethod enum.
 *
 * As mentioned, the SpiderSwarm uses Phenotypes. The Phenotype represent
 * one individual within the population. There will always be the same
 * number of Phenotypes as the PopulationSize is set to. There may be,
 * due to some rounding errors, become more than the PopulationSize, but
 * never less.
 *
 * A Phenotype handles the simulation of itself. It also has all the inputs
 * that is sent to the network and uses the outputs given.
 *
 * The SpiderSwarm will use mutlithreading on hte machine if BT_NO_PROFILE is
 * defined.
 */
class SpiderSwarm : Logging::Log {
public:

  //! This describes the different drawing methods
  //!
  //! - DrawSingleInBatch: Draw first one of the current batch
  //! - DrwaAllInBatch   : Draw all of the current batch
  //! - Species1         : Draw the first of each species
  //! - SpeciesLeaders   : Draw the best of each species
  //! - SpeciesLeaders   : Draw the best of the species
  //! - BestFitness      : Draw the best spider
  //! - DrawAll          : Draw all spiders
  //!
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

  // Toggles the drawing of the neural network for each spider
  void toggleDrawDebugNetworks();

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

  // Individual load functions
  void loadPopulation(const std::string& filename);
  void loadSubstrate(const std::string& filename);

  float currentDuration();
  float iterationDuration();
  void setIterationDuration(float x);

  // Returns a reference to the parameter
  NEAT::Parameters& parameters();

  // Returns a reference to the substrate
  NEAT::Substrate& substrate();

  // Returns the best genome for the current generation
  NEAT::Genome bestGenome();

  // Returns the best possible genome ever
  NEAT::Genome& bestPossibleGenome();

  // Restarts the simulation
  void restart();

private:
  std::vector<Phenotype> mPhenotypes;

  unsigned int mNumInputs;

  size_t mCurrentBatch;
  size_t mBatchStart;
  size_t mBatchEnd;
  size_t mBatchSize;
  size_t mGeneration;

  float mCurrentDuration;
  float mIterationDuration;
  float mBestPossibleFitness;
  unsigned int mBestPossibleFitnessGeneration;

  bool mDrawDebugNetworks;
  bool mRestartOnNextUpdate;

  Statistics mStats;

  NEAT::Genome mBestPossibleGenome;

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
                     Substrate&                       sub)>
    mBuildingHyperNeatWorker;
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
  Substrate*        mSubstrate;
  NEAT::Population* mPopulation;

  void setDefaultSubstrate();
  void setDefaultPopulation();
};
