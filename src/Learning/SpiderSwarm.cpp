#include "SpiderSwarm.hpp"

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"
#include "DrawablePhenotype.hpp"
#include "Substrate.hpp"

#include "../Experiments/Standing0102.hpp"
#include "../Experiments/Standing0304.hpp"
#include "../Experiments/Walking0102.hpp"
#include "../Experiments/Walking03.hpp"
#include "../Experiments/Walking04.hpp"
#include "../Experiments/Walking05.hpp"
#include "../Experiments/Walking07.hpp"
#include "../Experiments/Walking08.hpp"

#include <btBulletDynamicsCommon.h>
#include <thread>

#include <Genome.h>
#include <NeuralNetwork.h>
#include <Parameters.h>
#include <Population.h>
#include <Substrate.h>

/**
 * @brief
 *
 *
 */
static std::vector<mmm::vec3> grid = {
  { -4, 0, -4 }, { 4, 0, -4 },   { 4, 0, 4 },     { -4, 0, 4 },

  { -12, 0, 4 }, { -12, 0, -4 }, { -12, 0, -12 }, { -4, 0, -12 },

  { 4, 0, -12 }, { 12, 0, -12 }, { 12, 0, -4 },   { 12, 0, 4 },

  { 12, 0, 12 }, { 4, 0, 12 },   { -4, 0, 12 },   { -12, 0, 12 },
};

SpiderSwarm::SpiderSwarm()
    : Logging::Log("SpiderSwarm")
    , mCurrentBatch(0)
    , mBatchStart(0)
    , mBatchEnd(7)
    , mBatchSize(7)
    , mGeneration(0)
    , mCurrentDuration(0)
    , mBestPossibleFitness(-99999)
    , mBestPossibleFitnessGeneration(-99999)
    , mDrawDebugNetworks(false)
    , mRestartOnNextUpdate(false)
    , mSimulatingStage(SimulationStage::None)
    , mDrawingMethod(SpiderSwarm::DrawingMethod::Species1)
    , mBestIndex(0)
    , mSubstrate(nullptr)
    , mPopulation(nullptr)
    , mCurrentExperiment(nullptr) {

// Save some memory if bullet has profiling on and therefore
// does not allow for threading
#ifdef BT_NO_PROFILE
  mWorker = [](std::vector<Phenotype>::iterator begin,
               std::vector<Phenotype>::iterator end,
               const Experiment&                experiment) {
    for (auto it = begin; it != end; ++it)
      it->update(experiment);
  };

  mBuildingWorker = [](std::vector<Phenotype>::iterator begin,
                       std::vector<Phenotype>::iterator end,
                       const Experiment&                exp,
                       NEAT::Population&                pop,
                       Substrate&                       sub) {
    for (auto it = begin; it != end; ++it) {
      if (exp.parameters().useESHyperNEAT) {
        pop.m_Species[it->speciesIndex]
          .m_Individuals[it->individualIndex]
          .BuildESHyperNEATPhenotype(*it->network, sub, pop.m_Parameters);
      } else {
        pop.m_Species[it->speciesIndex]
          .m_Individuals[it->individualIndex]
          .BuildHyperNEATPhenotype(*it->network, sub);
      }
    }
  };
#endif

  NEAT::RNG rng;
  rng.TimeSeed();
}

/**
 * @brief
 *   Deletes all the data associated with the SpiderSwarm. If you want to save
 *   some information, it has to be done before you delete things
 */
SpiderSwarm::~SpiderSwarm() {
  for (auto& p : mPhenotypes)
    p.remove();

  delete mCurrentExperiment;
  mPhenotypes.clear();
}

/**
 * @brief
 *   Sets the new drawing method. This will update as soon as possible,
 *   latest during next epoch.
 *
 * @param dm
 */
void SpiderSwarm::setDrawingMethod(DrawingMethod dm) {
  mDrawingMethod = dm;
}

void SpiderSwarm::disableDrawing() {
  mDisableDrawing = true;
}

void SpiderSwarm::enableDrawing() {
  mDisableDrawing = false;
}

/**
 * @brief
 *   Setups an experiment by a given name
 *
 * @param name
 */
void SpiderSwarm::setup(const std::string& name, bool startExperiment) {
  stop();

  if (mCurrentExperiment != nullptr) {
    delete mCurrentExperiment;
    mCurrentExperiment = nullptr;
  }

  if (name == "Walking0102")
    mCurrentExperiment = new Walking0102();
  else if (name == "Walking04")
    mCurrentExperiment = new Walking04();
  else if (name == "Standing0102")
    mCurrentExperiment = new Standing0102();
  else if (name == "Standing0304")
    mCurrentExperiment = new Standing0304();
  else if (name == "Walking05")
    mCurrentExperiment = new Walking05();
  else if (name == "Walking03")
    mCurrentExperiment = new Walking03();
  else if (name == "Walking08")
    mCurrentExperiment = new Walking08();
  else if (name == "Walking07")
    mCurrentExperiment = new Walking07();
  else
    throw std::runtime_error("Unable to find experiment: " + name);

  mPopulation = mCurrentExperiment->population();
  mSubstrate  = mCurrentExperiment->substrate();

  if (mPopulation == nullptr)
    throw std::runtime_error("Population is not defined by experiment");

  if (mSubstrate == nullptr)
    throw std::runtime_error("Substrate is not defined by experiment");

  recreatePhenotypes();

  if (startExperiment)
    start();
}

/**
 * @brief
 *   Starts a simulation that has been setup
 */
void SpiderSwarm::start() {
  if (mCurrentExperiment == nullptr || mPopulation == nullptr ||
      mSubstrate == nullptr) {
    mLog->warn("Cannot start experiment without setting up experiment");
    return;
  }

  if (mSimulatingStage == SimulationStage::SimulationReady)
    mSimulatingStage = SimulationStage::Simulating;
  else if (mSimulatingStage == SimulationStage::None)
    mSimulatingStage = SimulationStage::Experiment;
}

/**
 * @brief
 *   Stops the current active experiment, if any
 */
void SpiderSwarm::stop() {
  if (mCurrentExperiment == nullptr || mPopulation == nullptr ||
      mSubstrate == nullptr) {
    mLog->warn("Cannot stop experiment without setting up experiment");
    return;
  }

  mSimulatingStage = SimulationStage::None;
}

/**
 * @brief
 *   This overwrites the currently stored bestGenome on the spiderswarm
 *   with a new open, allowing you to run this genome by using
 *   runBestGenome.
 *
 *   Assumes that the current loaded experiment is the population and substrate
 *   to use. Gives undefined results if this is not the case
 *
 *   Expects the filename to not contain the .genome extension
 *
 * @param name
 */
void SpiderSwarm::loadGenome(const std::string& filename) {
  if (mCurrentExperiment == nullptr) {
    mLog->debug("Must setup experiment before running genome");
    return;
  }

  std::string genomeFilename = filename + ".genome";

  // Load the best possible genome if its available.
  std::ifstream fs;
  fs.open(genomeFilename);

  if (fs.is_open()) {
    mBestPossibleGenome = NEAT::Genome(genomeFilename.c_str());
    mLog->info("Loaded new genome with id: {}", mBestPossibleGenome.GetID());
  } else {
    mLog->error("Unable to open genome file: {}", genomeFilename);
    throw std::runtime_error("Unable to open genome file: " + genomeFilename);
  }

  mSimulatingStage = SimulationStage::None;
  mCurrentDuration = 0;
}

/**
 * @brief
 *   When a new experiment is loaded from file (not setup, but using the load
 * function),
 *   it also loads all the genomes. This function allows you to simulate one of
 * those
 *   genomes
 *
 * @param genomeId
 */
void SpiderSwarm::runGenome(unsigned int genomeId) {
  if (mCurrentExperiment == nullptr) {
    mLog->debug("Must setup experiment before running genome");
    return;
  }

  NEAT::Genome* g = nullptr;

  if (genomeId == mBestPossibleGenome.GetID())
    g = &mBestPossibleGenome;
  else {
    for (unsigned int i = 0; i < mPopulation->m_Species.size(); i++) {
      for (unsigned int j = 0;
           j < mPopulation->m_Species[i].m_Individuals.size();
           j++) {
        if (mPopulation->m_Species[i].m_Individuals[j].GetID() == genomeId) {
          g = &mPopulation->m_Species[i].m_Individuals[j];
          break;
        }
      }
    }
  }

  if (g == nullptr) {
    mLog->error("Unable to find genome with Id: {}", genomeId);
    return;
  }

  mLog->info("Found genome, building network..");

  if (mPhenotypes.size() == 0)
    mPhenotypes.resize(1);

  mPhenotypes[0].reset(0, 0, 0, genomeId);
  mCurrentExperiment->initPhenotype(mPhenotypes[0]);

  if (mCurrentExperiment->parameters().useESHyperNEAT) {
    g->BuildESHyperNEATPhenotype(*mPhenotypes[0].network,
                                 *mSubstrate,
                                 mPopulation->m_Parameters);
  } else {
    g->BuildHyperNEATPhenotype(*mPhenotypes[0].network, *mSubstrate);
  }

  mSimulatingStage = SimulationStage::SimulationReady;
  mCurrentDuration = 0;
  mLog->info("Ready to simulate genome: {}", genomeId);
}

/**
 * @brief
 *   This is a shortcut for running the best genome that is loaded. This
 *   uses the runGenome function together with teh ID of the best stored
 *   genome.
 */
void SpiderSwarm::runBestGenome() {
  runGenome(mBestPossibleGenome.GetID());
}

/**
 * @brief
 *   Returns the current drawing method
 *
 * @return
 */
SpiderSwarm::DrawingMethod SpiderSwarm::drawingMethod() {
  return mDrawingMethod;
}

/**
 * @brief
 *   Toggles the drawing of the neural networks
 */
void SpiderSwarm::toggleDrawANN() {
  mDrawDebugNetworks = !mDrawDebugNetworks;

  if (mDrawDebugNetworks && mSimulatingStage == SimulationStage::Experiment) {
    for (auto& i : mPhenotypes) {
      i.drawablePhenotype->recreate(*i.network, mmm::vec3(1.0, 1.0, 1.0));
    }
  } else if (mDrawDebugNetworks &&
             (mSimulatingStage == SimulationStage::Simulating ||
              mSimulatingStage == SimulationStage::SimulationReady)) {
    mPhenotypes[0].drawablePhenotype->recreate(*mPhenotypes[0].network,
                                               mmm::vec3(1));
  }
}

void SpiderSwarm::updateSimulation() {
  if (mSimulatingStage == SimulationStage::SimulationReady &&
      mPhenotypes[0].duration > 0.0)
    return;

  mPhenotypes[0].failed = false;
  mPhenotypes[0].update(*mCurrentExperiment);
}

/**
 * @brief
 *   This is the main update function that controls what to do next.
 *   Most of the time this will update the spiders by activating the networks,
 *   using the networks output before running physics.
 *
 *   When a batch is complete, the next one is chosen. Once all batches are
 *   complete, the next epoch is started, resetting all the spiders to their
 *   original position.
 *
 *   If the restart flag is active, it will delete and clear everything before
 *   recreating the Phenotypes and returning, waiting until next update
 *   to start again.
 */
void SpiderSwarm::update(float deltaTime) {
  if (mSimulatingStage == SimulationStage::None)
    return;

  if (mSimulatingStage == SimulationStage::Simulating ||
      mSimulatingStage == SimulationStage::SimulationReady)
    return updateSimulation();

  if (mRestartOnNextUpdate) {
    for (auto& p : mPhenotypes)
      p.remove();

    mPhenotypes.clear();
    mBatchStart          = 0;
    mBatchEnd            = mBatchSize;
    mCurrentBatch        = 0;
    mCurrentDuration     = 0;
    mRestartOnNextUpdate = false;
    mSpeciesLeaders.clear();
    mBestIndex = 0;

    recreatePhenotypes();
    return;
  }

  deltaTime = 1.f / 60.f;

  if (deltaTime > 0.5)
    return;

  bool isWipeout = true;
  for (auto& p : mPhenotypes)
    if (!p.hasBeenKilled()) {
      isWipeout = false;
      break;
    }

  if (isWipeout)
    return updateEpoch();

#ifndef BT_NO_PROFILE
  if (mCurrentDuration == 0)
    mLog->debug("Processing {} individuals", mBatchEnd - mBatchStart);

  if (mCurrentDuration < mCurrentExperiment->totalDuration()) {
    updateNormal(deltaTime);
  } else if (mBatchEnd < mPhenotypes.size()) {
    setNextBatch();
  } else {
    updateEpoch();
  }

#else
  if (mCurrentDuration == 0)
    mLog->debug("Processing {} individuals", mPhenotypes.size());

  if (mCurrentDuration < mCurrentExperiment->totalDuration()) {
    updateThreadBatches(deltaTime);
  } else {
    updateEpoch();
  }

#endif
}

/**
 * @brief
 *   Draws the spiders depending on the DrawingMethod used. See the
 * documentation
 *   of DrawingMethod for information about how things are drawn.
 *
 *   You can change DrawingMethod by using `setDrawingMethod`
 *
 *   When drawing more than one spider, they are placed in a grid like
 * structure.
 *   They all really have position 0,0,0, but are offset slightly to make it
 *   easier to identify each one of them.
 *
 * @param prog
 * @param bindTexture
 */
void SpiderSwarm::draw(std::shared_ptr<Program>& prog, bool bindTexture) {
  if (mSimulatingStage == SimulationStage::None || mDisableDrawing)
    return;

  if (mSimulatingStage == SimulationStage::Simulating ||
      mSimulatingStage == SimulationStage::SimulationReady) {
    mPhenotypes[0].draw(prog, mmm::vec3(0, 0, 0), bindTexture);

    if (bindTexture && mDrawDebugNetworks) {
      mPhenotypes[0].drawablePhenotype->draw3D(mmm::vec3(0, 5, 0));
    }

    return;
  }

  size_t numPhenotypes = mPhenotypes.size();
  size_t gridIndex     = 0;
  size_t gridSize      = grid.size();

  switch (mDrawingMethod) {
    case DrawingMethod::DrawNone:
      break;

    // Draw only the first spider of the current batch
    case DrawingMethod::DrawSingleInBatch: {
      if (mBatchStart < mPhenotypes.size()) {
        mPhenotypes[mBatchStart].draw(prog, grid[0], bindTexture);

        if (bindTexture && mDrawDebugNetworks)
          mPhenotypes[mBatchStart].drawablePhenotype->draw3D(
            grid[gridIndex] + mmm::vec3(0, 5, 0));
      }
      break;
    }

    // Draw all in current batch
    case DrawingMethod::DrawAllInBatch: {
      for (unsigned int i = mBatchStart;
           i < mBatchEnd && i < mPhenotypes.size() && i < gridSize;
           i++) {

        mPhenotypes[i].draw(prog, grid[0], bindTexture);

        if (bindTexture && mDrawDebugNetworks)
          mPhenotypes[i].drawablePhenotype->draw3D(grid[gridIndex] +
                                                   mmm::vec3(0, 5, 0));
      }
      break;
    }

    // Draw the first of each species
    case DrawingMethod::Species1: {
      for (auto& a : mPhenotypes) {
        if (a.speciesIndex == gridIndex && gridIndex < gridSize) {
          a.draw(prog, grid[gridIndex], bindTexture);

          if (bindTexture && mDrawDebugNetworks)
            a.drawablePhenotype->draw3D(grid[gridIndex] + mmm::vec3(0, 5, 0));
          gridIndex++;
        }
      }

      break;
    }

    // Draw the leaders of the species
    case DrawingMethod::SpeciesLeaders: {
      for (auto& a : mSpeciesLeaders) {
        if (a < numPhenotypes && gridIndex < gridSize) {
          mPhenotypes[a].draw(prog, grid[gridIndex], bindTexture);

          if (bindTexture && mDrawDebugNetworks)
            mPhenotypes[a].drawablePhenotype->draw3D(grid[gridIndex] +
                                                     mmm::vec3(0, 5, 0));
        }

        gridIndex++;
      }

      break;
    }

    // Draw the one with the best fitness in the last generation
    case DrawingMethod::BestFitness:
      if (mBestIndex < numPhenotypes) {
        mPhenotypes[mBestIndex].draw(prog, mmm::vec3(0, 0, 0), bindTexture);
        if (bindTexture && mDrawDebugNetworks)
          mPhenotypes[mBestIndex].drawablePhenotype->draw3D(grid[gridIndex] +
                                                            mmm::vec3(0, 5, 0));
      }
      break;

    // Draw all spiders
    case DrawingMethod::DrawAll:
      for (auto& p : mPhenotypes) {
        if (gridIndex < gridSize) {
          p.draw(prog, grid[gridIndex], bindTexture);
          if (bindTexture && mDrawDebugNetworks) {
            p.drawablePhenotype->draw3D(grid[gridIndex] + mmm::vec3(0, 5, 0));
          }
        }
        gridIndex++;
      }
      break;
  }
}

/**
 * @brief
 *   Saves the SpiderSwarm to several files, by using a common
 *   filename. It will save three files representing:
 *
 *   - Population
 *   - Substrate
 *   - Statistics
 *   - Best Possible Genome
 *
 *   The filename convention will be:
 *
 *   - `filename.population`
 *   - `filename.substrate`
 *   - `filename.csv`
 *   - `filename.genome`
 *
 *   The parameters are saved in the population.
 *
 * @param filename
 */
void SpiderSwarm::save(const std::string& filename) {
  if (mCurrentExperiment == nullptr) {
    mLog->warn("You must have an experiment loaded before saving");
    return;
  }

  std::string popFilename    = filename + ".population";
  std::string subFilename    = filename + ".substrate";
  std::string genomeFilename = filename + ".genome";
  std::string statsFilename  = filename + ".csv";

  mPopulation->Save(popFilename.c_str());
  mSubstrate->save(subFilename);
  mBestPossibleGenome.Save(genomeFilename.c_str());
  mStats.save(statsFilename);
}

/**
 * @brief
 *   Loads the SpiderSwarm from several files indicated
 *   by the filename. Just like save, it will expect several files.
 *
 *   If filename is `mySpiderSwarm`, it will expect to find
 *   three different files:
 *
 *   - `mySpiderSwarm.parameters`
 *   - `mySpiderSwarm.substrate`
 *
 *   It will also look for:
 *
 *   - `mySpiderSwarm.genome`
 *
 *   but this one is not required.
 *
 *   **Note**: This also resets the current values to their
 *   initial values:
 *
 *   - mCurrentBatch
 *   - mBestIndex
 *   - mCurrentDuration
 *   - mBestPossibleFitness
 *
 *   And it will retrieve the generation count from
 *   the population
 *
 * @param filename
 */
void SpiderSwarm::load(const std::string& filename) {
  if (mCurrentExperiment == nullptr) {
    mLog->warn("You must load experiment before loading file");
    return;
  }

  std::string popFilename    = filename + ".population";
  std::string paramFilename  = filename + ".parameters";
  std::string subFilename    = filename + ".substrate";
  std::string genomeFilename = filename + ".genome";

  mPopulation = new NEAT::Population(popFilename.c_str());
  mSubstrate->load(subFilename);

  // Load the best possible genome if its available.
  std::ifstream fs;
  fs.open(genomeFilename);

  if (fs.is_open()) {
    mBestPossibleGenome = NEAT::Genome(genomeFilename.c_str());
  } else {
    mBestPossibleGenome = bestGenome();
  }

  mCurrentBatch                  = 0;
  mBestIndex                     = 0;
  mGeneration                    = mPopulation->m_Generation;
  mCurrentDuration               = 0;
  mBestPossibleFitness           = 0;
  mBestPossibleFitnessGeneration = 0;
  mStats                         = Statistics();
  mSimulatingStage               = SimulationStage::None;

  mLog->info("Loaded from file: {}", filename);
  mLog->info("Ready to start experiment");
}

/**
 * @brief
 *   Returns the current iteration duration
 *   that is incremented with 1/60th of a second every
 *   time the update loop is executed.
 *
 * @return
 */
float SpiderSwarm::currentDuration() {
  return mCurrentDuration;
}

/**
 * @brief
 *   Returns const reference to a list of Phenotypes
 *
 * @return
 */
const std::vector<Phenotype>& SpiderSwarm::phenotypes() {
  return mPhenotypes;
}

/**
 * @brief
 *   Returns the current stage of the SpiderSwarm
 *
 * @return
 */
SpiderSwarm::SimulationStage SpiderSwarm::stage() {
  return mSimulatingStage;
}

/**
 * @brief
 *   If BT_NO_PROFILE is defined, it will assume that we are to use
 *   multithreading. Unlike `updateUsingThreads` it does not split the current
 *   batch into several smaller pieces, but instead splits the entire number
 *   of Phenotypes into smaller batches.
 *
 *   For instance, if you have 4 threads and a population size of 50, it will
 *   split 12 Phenotypes to 3 of the threads and the last one will have work
 *   with the remaining 14.
 *
 * @param deltaTime
 */
#ifdef BT_NO_PROFILE
void SpiderSwarm::updateThreadBatches(float deltaTime) {

  int size    = mPhenotypes.size();
  int nThread = mmm::min(std::thread::hardware_concurrency(), size);
  std::vector<std::thread> threads(nThread);

  auto workIter  = std::begin(mPhenotypes);
  int  grainSize = size / threads.size();

  for (auto it = std::begin(threads); it != std::end(threads) - 1; ++it) {
    *it = std::thread(mWorker,
                      workIter,
                      workIter + grainSize,
                      std::ref(*mCurrentExperiment));
    workIter += grainSize;
  }

  threads.back() = std::thread(mWorker,
                               workIter,
                               std::end(mPhenotypes),
                               std::ref(*mCurrentExperiment));

  for (auto&& i : threads)
    i.join();

  mCurrentDuration += deltaTime;
}
#else
void  SpiderSwarm::updateThreadBatches(float) {}
#endif

/**
 * @brief
 *   Works exactly like `updateNormal` but instead of performing everything in
 *   a single thread, it runs multiple threads (as many as the system can
 *   support, returned by std::thread::hardware_concurrency or the max
 *   number in the batch size)
 *
 *   Please keep in mind that this will not work if BT_NO_PROFILE is not
 *   defined and it will fall back to using `updateNormal`. The same
 *   goes if the system does not support threading (where the
 *   std::thread:;hardware_concurrency returns 0)
 *
 * @param deltaTime
 */
void SpiderSwarm::updateUsingThreads(float deltaTime) {

// If Bullet profiling is on, we cant multithread.
#ifndef BT_NO_PROFILE
  return updateNormal(deltaTime);
#else
  int size    = mBatchEnd - mBatchStart;
  int nThread = mmm::min(std::thread::hardware_concurrency(), size);
  // If batchsize == 0 or hardware_concurrency returns 0, do nothing
  if (nThread <= 0)
    return updateNormal(deltaTime);

  std::vector<std::thread> threads(nThread);
  int                      grainSize = size / threads.size();
  auto                     workIter  = std::begin(mPhenotypes) + mBatchStart;

  for (auto it = std::begin(threads); it != std::end(threads) - 1; ++it) {
    *it = std::thread(mWorker,
                      workIter,
                      workIter + grainSize,
                      std::ref(*mCurrentExperiment));
    workIter += grainSize;
  }

  threads.back() = std::thread(mWorker,
                               workIter,
                               std::begin(mPhenotypes) + mBatchEnd,
                               std::ref(*mCurrentExperiment));

  for (auto&& i : threads)
    i.join();

  mCurrentDuration += deltaTime;
#endif
}

/**
 * @brief
 *   Goes through the current patch and performs the following on it:
 *
 *   1. Activate each network and use output to set information on the spider
 *   2. Perform physics to see the changes of the given output
 *   3. Update fitness based on how well it did.
 *
 *   See `updateUsingThreads` for the threaded version
 *
 * @param deltaTime
 */
void SpiderSwarm::updateNormal(float deltaTime) {
  for (size_t i = mBatchStart; i < mBatchEnd && i < mPhenotypes.size(); ++i) {
    mPhenotypes[i].update(*mCurrentExperiment);
  }
  mCurrentDuration += deltaTime;
}

/**
 * @brief
 *   Called when the current batch has been processed, setting the current
 *   batch to the next in line.
 */
void SpiderSwarm::setNextBatch() {
  mLog->debug("Complete batch {}", mCurrentBatch);

  mCurrentDuration = 0;
  mCurrentBatch += 1;

  mBatchStart = mCurrentBatch * mBatchSize;
  mBatchEnd   = mmm::min((mCurrentBatch + 1) * mBatchSize, mPhenotypes.size());
}

/**
 * @brief
 *   When all batches are completed, new spiders and networks has to be
 *   initialied.
 *
 *   This goes through all of the popluation, reseting the spider,
 *   world, network and fitness
 */
void SpiderSwarm::updateEpoch() {
  mLog->debug("Complete epoch");

  mCurrentDuration = 0;
  mCurrentBatch    = 0;
  mBatchStart      = 0;
  mBatchEnd        = mmm::min(mBatchSize, mPhenotypes.size());

  mSpeciesLeaders.clear();

  float  best        = -99999.f;
  size_t bestIndex   = 0;
  bool   changedBest = false;

  ++mGeneration;

  size_t index = 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    float  bestOfSpecies = -99999.f;
    size_t leaderIndex   = 0;

    for (size_t j = 0; j < mPopulation->m_Species[i].m_Individuals.size();
         ++j) {

      float fitness = mPhenotypes[index].finalizeFitness(*mCurrentExperiment);
      mPopulation->m_Species[i].m_Individuals[j].SetFitness(fitness);
      mPopulation->m_Species[i].m_Individuals[j].SetEvaluated();

      // Save the best fitness and its index so it can
      // be drawn on the next generation
      if (fitness > best) {
        best      = fitness;
        bestIndex = index;
      }

      // Save the best possible genome
      if (fitness > mBestPossibleFitness) {
        mBestPossibleFitness = best;
        mBestPossibleGenome  = mPopulation->m_Species[i].m_Individuals[j];
        mBestPossibleFitnessGeneration = mGeneration;
        changedBest                    = true;
      }

      if (fitness > bestOfSpecies) {
        bestOfSpecies = fitness;
        leaderIndex   = index;
      }

      index += 1;
    }

    mSpeciesLeaders.push_back(leaderIndex);
  }

  mBestIndex = bestIndex;

  mStats.addEntry(mPhenotypes, mGeneration);

  if (changedBest) {
    // also store it to file for future reference
    save("current-g" + std::to_string(mGeneration));
  }

  // Log some information about the generation that just finished
  // excuting, such as the best fitness overall and the best
  // for each species, together with the individual fitness values
  mLog->info("Generation {}", mGeneration);
  mLog->info("Best of current generation {}, Best of all: {} ({})",
             best,
             mBestPossibleFitness,
             mBestPossibleFitnessGeneration);

  for (auto i : mSpeciesLeaders) {
    if (i >= mPhenotypes.size())
      continue;

    Phenotype& p = mPhenotypes[i];

    if (!p.hasFinalized)
      p.finalizeFitness(*mCurrentExperiment);

    mLog->info("-------------------------------------");
    mLog->info("The best in species: {}-{} >>= {}{}{}",
               p.speciesId,
               p.individualIndex,
               p.finalizedFitness,
               i == mBestIndex ? " (best)" : "",
               p.hasBeenKilled() ? " (killed)" : "");

    size_t       j         = 0;
    unsigned int maxLength = 0;

    // Find the longest name so alignment can be done
    for (auto& f : mCurrentExperiment->fitnessFunctions())
      maxLength = mmm::max(f.name().size(), maxLength);

    // Print out aligned fitness names and their values
    for (auto& f : mCurrentExperiment->fitnessFunctions()) {
      std::string name = f.name();

      if (name.size() < maxLength)
        name.append(maxLength - name.size(), ' ');

      mLog->info("  {}: {}", name, p.fitness[j]);
      j += 1;
    }
  }

  mPopulation->Epoch();
  recreatePhenotypes();

  // mLog->info("saving population...");
  // save("testswarm");
}

/**
 * @brief
 *   After an Epoch has been completed, the Phenotypes have to be reset and
 *   each Neural Network has to be rebuilt.
 *
 *   This function goes through all species and individuals and resets
 *   the Phenotype that represents each individual. If there are more
 *   individuals than the amount of Phenotypes, additional Phenotypes
 *   are added.
 *
 *   Similarly, if there are less individuals than the number of
 *   Phenotypes, Phenotypes are removed.
 *
 *   If BT_NO_PROFILE is true, it will generated the ESHyperNEAT phenotypes
 *   using multithreading, splitting up the workload much like
 *   `updateThreadBatches`
 */
void SpiderSwarm::recreatePhenotypes() {
  mLog->debug("Recreating {} phenotypes...",
              mPopulation->m_Parameters.PopulationSize);
  mLog->debug("We have {} species", mPopulation->m_Species.size());

  size_t index      = 0;
  bool   addLeaders = mSpeciesLeaders.size() == 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    auto& species = mPopulation->m_Species[i];

    if (addLeaders) {
      mSpeciesLeaders.push_back(index);
    }

    for (size_t j = 0; j < species.m_Individuals.size(); ++j) {
      NEAT::Genome& g = species.m_Individuals[j];

      // Add new element if the population size has increased
      if (index >= mPhenotypes.size()) {
        mLog->debug("Adding new spider due to increase in population");
        mPhenotypes.push_back(Phenotype());
      }

      mPhenotypes[index].reset(species.ID(), i, j, g.GetID());
      mPhenotypes[index].spider->disableUpdatingFromPhysics();
      mCurrentExperiment->initPhenotype(mPhenotypes[index]);

// If we are using single-threaded mode, create the neural
// networks, otherwise wait until later
#ifndef BT_NO_PROFILE
      auto& individual = species.m_Individuals[j];
      if (mCurrentExperiment->parameters().useESHyperNEAT) {
        individual.BuildESHyperNEATPhenotype(*mPhenotypes[index].network,
                                             *mSubstrate,
                                             mPopulation->m_Parameters);
      } else {
        individual.BuildHyperNEATPhenotype(*mPhenotypes[index].network,
                                           *mSubstrate);
      }
#endif
      ++index;
    }
  }

  // Remove the excess phenotypes
  while (index < mPhenotypes.size()) {
    mPhenotypes.back().remove();
    mPhenotypes.pop_back();
    mLog->debug("Removing spider due to decrease in population");
  }

// If using multithreaded more, generated the ESHyperNEAT neural
// networks in paralell
#ifdef BT_NO_PROFILE
  int size    = mPhenotypes.size();
  int nThread = mmm::min(std::thread::hardware_concurrency(), size);
  std::vector<std::thread> threads(nThread);

  auto workIter  = std::begin(mPhenotypes);
  int  grainSize = size / threads.size();

  mLog->debug("Building networks with {} thread(s)", nThread);

  for (auto it = std::begin(threads); it != std::end(threads) - 1; ++it) {
    *it = std::thread(mBuildingWorker,
                      workIter,
                      workIter + grainSize,
                      std::ref(*mCurrentExperiment),
                      std::ref(*mPopulation),
                      std::ref(*mSubstrate));
    workIter += grainSize;
  }

  threads.back() = std::thread(mBuildingWorker,
                               workIter,
                               std::end(mPhenotypes),
                               std::ref(*mCurrentExperiment),
                               std::ref(*mPopulation),
                               std::ref(*mSubstrate));

  for (auto&& i : threads)
    i.join();

#endif

  // If we want to see the Networks, create those
  // after the networks have been added
  if (mDrawDebugNetworks) {
    for (auto& i : mPhenotypes) {
      i.drawablePhenotype->recreate(*i.network, mmm::vec3(1.0, 1.0, 1.0));
    }
  }

  mLog->debug("Created {} spiders", mPhenotypes.size());
}

/**
 * @brief
 *   Returns a reference to the parameters
 *
 * @return
 */
NEAT::Parameters& SpiderSwarm::parameters() {
  return mPopulation->m_Parameters;
}

/**
 * @brief
 *   Returns a reference to the substrate
 *
 * @return
 */
NEAT::Substrate& SpiderSwarm::substrate() {
  return *mSubstrate;
}

/**
 * @brief
 *   Returns the best genome for the current
 *   generation
 *
 * @return
 */
NEAT::Genome SpiderSwarm::bestGenome() {
  return mPopulation->GetBestGenome();
}

/**
 * @brief
 *   Returns the best possible genome since the start
 *   of the simulation
 *
 * @return
 */
NEAT::Genome& SpiderSwarm::bestPossibleGenome() {
  return mBestPossibleGenome;
}

/**
 * @brief
 *   Sets the restart flag on, which will
 *   restart the SpiderSwarm at the next update
 */
void SpiderSwarm::restart() {
  mRestartOnNextUpdate = true;
}
