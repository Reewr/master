#include "SpiderSwarm.hpp"

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"
#include "DrawablePhenotype.hpp"
#include "Substrate.hpp"

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
    , mIterationDuration(12)
    , mBestPossibleFitness(0)
    , mBestPossibleFitnessGeneration(0)
    , mDrawDebugNetworks(true)
    , mRestartOnNextUpdate(false)
    , mDrawingMethod(SpiderSwarm::DrawingMethod::Species1)
    , mBestIndex(0)
    , mSubstrate(nullptr)
    , mPopulation(nullptr) {

// Save some memory if bullet has profiling on and therefore
// does not allow for threading
#ifdef BT_NO_PROFILE
  mWorker = [](std::vector<Phenotype>::iterator begin,
               std::vector<Phenotype>::iterator end,
               float                            deltaTime) {
    for (auto it = begin; it != end; ++it)
      it->update(deltaTime);
  };

  mBuildingESHyperNeatWorker = [](std::vector<Phenotype>::iterator begin,
                                  std::vector<Phenotype>::iterator end,
                                  NEAT::Population&                pop,
                                  Substrate&                       sub,
                                  NEAT::Parameters&                params) {
    for (auto it = begin; it != end; ++it) {
      pop.m_Species[it->speciesIndex]
        .m_Individuals[it->individualIndex]
        .BuildESHyperNEATPhenotype(*it->network, sub, params);
    }
  };
#endif

  NEAT::RNG rng;
  rng.TimeSeed();

  setDefaultSubstrate();
  setDefaultPopulation();

  mPhenotypes.reserve(mPopulation->m_Parameters.PopulationSize);

  recreatePhenotypes();
}

/**
 * @brief
 *   Deletes all the data associated with the SpiderSwarm. If you want to save
 *   some information, it has to be done before you delete things
 */
SpiderSwarm::~SpiderSwarm() {
  for (auto& p : mPhenotypes)
    p.remove();

  mPhenotypes.clear();
  delete mSubstrate;
  delete mPopulation;
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
void SpiderSwarm::toggleDrawDebugNetworks() {
  mDrawDebugNetworks = !mDrawDebugNetworks;

  if (mDrawDebugNetworks) {
    for (auto& i : mPhenotypes) {
      i.drawablePhenotype->recreate(*i.network, mmm::vec3(1.0, 1.0, 1.0));
    }
  }
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
  if (mRestartOnNextUpdate) {
    for (auto& p : mPhenotypes)
      p.remove();

    mPhenotypes.clear();
    mBatchStart = 0;
    mBatchEnd = mBatchSize;
    mCurrentBatch = 0;
    mCurrentDuration = 0;
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
  for(auto& p : mPhenotypes)
    if (!p.hasBeenKilled()) {
      isWipeout = false;
      break;
    }

  if (isWipeout)
    return updateEpoch();

#ifndef BT_NO_PROFILE
  if (mCurrentDuration == 0)
    mLog->debug("Processing {} individuals", mBatchEnd - mBatchStart);

  if (mCurrentDuration < mIterationDuration) {
    updateNormal(deltaTime);
  } else if (mBatchEnd < mPhenotypes.size()) {
    setNextBatch();
  } else {
    updateEpoch();
  }

#else
  if (mCurrentDuration == 0)
    mLog->debug("Processing {} individuals", mPhenotypes.size());

  if (mCurrentDuration < mIterationDuration) {
    updateThreadBatches(deltaTime);
  } else {
    updateEpoch();
  }

#endif
}

/**
 * @brief
 *   Draws the spiders depending on the DrawingMethod used. See the documentation
 *   of DrawingMethod for information about how things are drawn.
 *
 *   You can change DrawingMethod by using `setDrawingMethod`
 *
 *   When drawing more than one spider, they are placed in a grid like structure.
 *   They all really have position 0,0,0, but are offset slightly to make it
 *   easier to identify each one of them.
 *
 * @param prog
 * @param bindTexture
 */
void SpiderSwarm::draw(std::shared_ptr<Program>& prog, bool bindTexture) {
  size_t numPhenotypes = mPhenotypes.size();
  size_t gridIndex     = 0;
  size_t gridSize      = grid.size();

  switch (mDrawingMethod) {
    // Draw only the first spider of the current batch
    case DrawingMethod::DrawSingleInBatch: {
      if (mBatchStart < mPhenotypes.size()) {
        mPhenotypes[mBatchStart].draw(prog, grid[0], bindTexture);

        if (bindTexture && mDrawDebugNetworks)
          mPhenotypes[mBatchStart].drawablePhenotype->draw3D(grid[gridIndex] + mmm::vec3(0, 5, 0));
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
        mPhenotypes[mBestIndex].draw(prog, mmm::vec3(0,0,0), bindTexture);
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
 *   - Parameters
 *   - Substrate
 *
 *   The filename convention will be:
 *
 *   - `filename.population`
 *   - `filename.parameters`
 *   - `filename.substrate`
 *
 * @param filename
 */
void SpiderSwarm::save(const std::string& filename) {
  std::string popFilename    = filename + ".population";
  std::string paramFilename  = filename + ".parameters";
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
 *   - `mySpiderSwarm.population`
 *   - `mySpiderSwarm.parameters`
 *   - `mySpiderSwarm.substrate`
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
  std::string popFilename    = filename + ".population";
  std::string paramFilename  = filename + ".parameters";
  std::string subFilename    = filename + ".substrate";
  std::string genomeFilename = filename + ".genome";

  if (mPopulation != nullptr)
    delete mPopulation;

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

  mCurrentBatch        = 0;
  mBestIndex           = 0;
  mGeneration          = mPopulation->m_Generation;
  mCurrentDuration     = 0;
  mBestPossibleFitness = 0;
  mBestPossibleFitnessGeneration = 0;

  recreatePhenotypes();
}

/**
 * @brief
 *   Like `load`, but only loads the population from file. The population
 *   does actually contain its stored parameters, so it also loads that
 *   from file.
 *
 * @param filename
 */
void SpiderSwarm::loadPopulation(const std::string& filename) {
  std::string popFilename   = filename + ".population";

  if (mPopulation != nullptr)
    delete mPopulation;

  mPopulation = new NEAT::Population(popFilename.c_str());

  mCurrentBatch        = 0;
  mBestIndex           = 0;
  mGeneration          = mPopulation->m_Generation;
  mCurrentDuration     = 0;
  mBestPossibleFitness = 0;

  recreatePhenotypes();
}

/**
 * @brief
 *   Like `load` but only loads the substrate from file. Like
 *   the other load functions, it basically resets everything.
 *
 * @param filename
 */
void SpiderSwarm::loadSubstrate(const std::string& filename) {
  std::string subFilename   = filename + ".substrate";

  mSubstrate->load(subFilename);

  mCurrentBatch        = 0;
  mBestIndex           = 0;
  mGeneration          = mPopulation->m_Generation;
  mCurrentDuration     = 0;
  mBestPossibleFitness = 0;

  recreatePhenotypes();
}

float SpiderSwarm::currentDuration() {
  return mCurrentDuration;
}
float SpiderSwarm::iterationDuration() {
  return mIterationDuration;
}
void SpiderSwarm::setIterationDuration(float x) {
  mIterationDuration = x;
}

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
    *it = std::thread(mWorker, workIter, workIter + grainSize, deltaTime);
    workIter += grainSize;
  }

  threads.back() = std::thread(mWorker,
                               workIter,
                               std::begin(mPhenotypes) + mBatchEnd,
                               deltaTime);

  for (auto&& i : threads)
    i.join();

  mCurrentDuration += deltaTime;
#endif
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
    *it = std::thread(mWorker, workIter, workIter + grainSize, deltaTime);
    workIter += grainSize;
  }

  threads.back() =
    std::thread(mWorker, workIter, std::end(mPhenotypes), deltaTime);

  for (auto&& i : threads)
    i.join();

  mCurrentDuration += deltaTime;
}
#else
void SpiderSwarm::updateThreadBatches(float) {}
#endif

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
    mPhenotypes[i].update(deltaTime);
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

  float  best      = -1.f;
  size_t bestIndex = 0;

  size_t index = 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    float  bestOfSpecies = -1.0;
    size_t leaderIndex   = 0;

    for (size_t j = 0; j < mPopulation->m_Species[i].m_Individuals.size();
         ++j) {

      float fitness = mPhenotypes[index].finalizeFitness();
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
        mBestPossibleFitnessGeneration = mGeneration + 1;
      }

      if (fitness > bestOfSpecies) {
        bestOfSpecies = fitness;
        leaderIndex   = index;
      }

      index += 1;
    }

    mSpeciesLeaders.push_back(leaderIndex);
  }

  ++mGeneration;
  mBestIndex = bestIndex;

  mStats.addEntry(mPhenotypes, mGeneration);

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
      p.finalizeFitness();

    mLog->info("-------------------------------------");
    mLog->info("The best in species: {}-{} >>= {}{}{}",
               p.speciesIndex,
               p.individualIndex,
               p.fitness[0],
               i == mBestIndex ? " (best)" : "",
               p.hasBeenKilled() ? " (killed)" : "");

    size_t j = 0;
    for (auto& f : Phenotype::FITNESS_HANDLERS) {
      mLog->info("  {}: {}", f.name(), p.fitness[j]);
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
  mLog->debug("Recreating {} phenotypes...", mPopulation->m_Parameters.PopulationSize);
  mLog->debug("We have {} species", mPopulation->m_Species.size());

  if (mNumInputs != mSubstrate->m_input_coords.size())
    mNumInputs = mSubstrate->m_input_coords.size();

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

      mPhenotypes[index].reset(i, j, g.GetID(), mNumInputs);
      mPhenotypes[index].spider->disableUpdatingFromPhysics();

      // If we are using single-threaded mode, create the neural
      // networks, otherwise wait until later
#ifndef BT_NO_PROFILE
      auto& individual = species.m_Individuals[j];
      individual.BuildESHyperNEATPhenotype(*mPhenotypes[index].network,
                                           *mSubstrate,
                                           mPopulation->m_Parameters);
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

  for (auto it = std::begin(threads); it != std::end(threads) - 1; ++it) {
    *it = std::thread(mBuildingESHyperNeatWorker,
                      workIter,
                      workIter + grainSize,
                      std::ref(*mPopulation),
                      std::ref(*mSubstrate),
                      std::ref(mPopulation->m_Parameters));
    workIter += grainSize;
  }

  threads.back() = std::thread(mBuildingESHyperNeatWorker,
                               workIter,
                               std::end(mPhenotypes),
                               std::ref(*mPopulation),
                               std::ref(*mSubstrate),
                               std::ref(mPopulation->m_Parameters));

  for (auto&& i : threads)
    i.join();

#endif

  /* mLog->debug("{} - {} has {} connections", */
  /*     mPhenotypes[0].speciesIndex, */
  /*     mPhenotypes[0].individualIndex, */
  /*     mPhenotypes[0].network->m_connections.size()); */
  /* for(auto& c : mPhenotypes[0].network->m_connections) { */
  /*   mLog->debug("Connection: {}:{} -> {}:{} x {} - Recurrent", */
  /*       c.m_source_neuron_idx, */
  /*       mPhenotypes[0].network->m_neurons[c.m_source_neuron_idx].m_type, */
  /*       c.m_target_neuron_idx, */
  /*       mPhenotypes[0].network->m_neurons[c.m_target_neuron_idx].m_type, */
  /*       c.m_weight, */
  /*       c.m_recur_flag); */
  /* } */

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

void SpiderSwarm::setDefaultSubstrate() {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  // clang-format off
  std::vector<std::vector<double>> inputs{
    { 0.000000, 0.000000, 0.000000 },    // Phase

    /* { 1.553501, -1.265974, 0.088832 },   // L1Phase */
    /* { 1.711714, -0.431291, 0.088831 },   // L2Phase */
    /* { 1.686564, 0.544734, 0.088831 },    // L3Phase */
    /* { 1.420332, 1.456160, 0.088831 },    // L4Phase */
    /* { -1.553502, -1.265975, 0.088831 },  // R1Phase */
    /* { -1.711714, -0.431291, 0.088831 },  // R2Phase */
    /* { -1.686564, 0.544734, 0.088831 },   // R3Phase */
    /* { -1.420332, 1.456160, 0.088831 },   // R4Phase */

    {  2.84, -2.0, -1.04 }, // L1Tip
    {  3.17, -0.6, -1.04 }, // L2Tip
    {  3.12,  0.9, -1.04 }, // L3Tip
    {  2.55,  2.4, -1.04 }, // L4Tip

    { -2.84, -2.0, -1.04 }, // R1Tip
    { -3.17, -0.6, -1.04 }, // R2Tip
    { -3.12,  0.9, -1.04 }, // R3Tip
    { -2.55,  2.4, -1.04 }, // R4Tip

    // Ang: 135, Rot: 44
    /* { -0.100000, 1.565926, 0.258819 },   // AbdominImpAngX */
    /* { 0.100000, 1.565926, 0.258819 },    // AbdominImpAngY */
    /* { 0.000000, 1.540044, 0.355412 },    // AbdominImpAngZ */
    /* { 0.000000, 0.898278, 0.079924 },    // AbdominImpRot */
    /* { -0.100000, -0.850000, 0.446689 },  // EyeImpAngX */
    /* { 0.100000, -0.850000, 0.446689 },   // EyeImpAngY */
    /* { 0.000000, -0.850000, 0.546689 },   // EyeImpAngZ */
    /* { 0.000000, -0.850000, 0.242041 },   // EyeImpRot */

    { 0.95, -1.03, 0.32 },   // FemurL1Collision
    { 1.05, -0.86, 0.32 },   // FemurL1Height
    { 0.95, -0.91, 0.40 },   // FemurL1POS X
    { 0.96, -0.92, 0.40 },   // FemurL1POS Y
    { 0.97, -0.93, 0.40 },   // FemurL1POS Z
    { 0.951, -0.911, 0.40 }, // FemurL1ANG X
    { 0.961, -0.921, 0.40 }, // FemurL1ANG Y
    { 0.971, -0.931, 0.40 }, // FemurL1ANG Z
    { 0.952, -0.912, 0.40 }, // FemurL1VEL X
    { 0.962, -0.922, 0.40 }, // FemurL1VEL Y
    { 0.972, -0.932, 0.40 }, // FemurL1VEL Z
    { 0.60, -0.72, 0.00 },   // FemurL1Rot

    { 1.07, -0.42, 0.32 },   // FemurL2ImpAngX
    { 1.10, -0.22, 0.32 },   // FemurL2ImpAngY
    { 1.03, -0.31, 0.40 },   // FemurL2 POS X
    { 1.04, -0.32, 0.40 },   // FemurL2 POS Y
    { 1.05, -0.33, 0.40 },   // FemurL2 POS Z
    { 1.031, -0.311, 0.40 }, // FemurL2 ANG X
    { 1.041, -0.321, 0.40 }, // FemurL2 ANG Y
    { 1.051, -0.331, 0.40 }, // FemurL2 ANG Z
    { 1.032, -0.312, 0.40 }, // FemurL2 VEL X
    { 1.042, -0.322, 0.40 }, // FemurL2 VEL Y
    { 1.052, -0.332, 0.40 }, // FemurL2 VEL Z
    { 0.63, -0.24, 0.00 },   // FemurL2ImpRot

    { 1.10, 0.28, 0.32 },    // FemurL3ImpAngX
    { 1.05, 0.47, 0.32 },    // FemurL3ImpAngY
    { 1.02, 0.36, 0.40 },    // FemurL3 POS X
    { 1.03, 0.37, 0.40 },    // FemurL3 POS Y
    { 1.04, 0.38, 0.40 },    // FemurL3 POS Z
    { 1.021, 0.361, 0.40 },  // FemurL3 ANG X
    { 1.031, 0.371, 0.40 },  // FemurL3 ANG Y
    { 1.041, 0.381, 0.40 },  // FemurL3 ANG Z
    { 1.021, 0.362, 0.40 },  // FemurL3 VEL X
    { 1.031, 0.372, 0.40 },  // FemurL3 VEL Y
    { 1.041, 0.382, 0.40 },  // FemurL3 VEL Z
    { 0.63, 0.26, 0.00 },    // FemurL3ImpRot

    { 1.00, 0.97, 0.32 },    // FemurL4Collision
    { 0.87, 1.12, 0.32 },    // FemurL4Height
    { 0.89, 1.01, 0.40 },    // FemurL4 POS X
    { 0.90, 1.02, 0.40 },    // FemurL4 POS Y
    { 0.91, 1.03, 0.40 },    // FemurL4 POS Z
    { 0.891, 1.011, 0.40 },  // FemurL4 ANG X
    { 0.901, 1.021, 0.40 },  // FemurL4 ANG Y
    { 0.911, 1.031, 0.40 },  // FemurL4 ANG Z
    { 0.891, 1.012, 0.40 },  // FemurL4 VEL X
    { 0.901, 1.022, 0.40 },  // FemurL4 VEL Y
    { 0.911, 1.032, 0.40 },  // FemurL4 VEL Z
    { 0.58, 0.75, 0.00 },    // FemurL4ImpRot

    { -0.95, -1.03, 0.32 },  // FemurR1Collision
    { -1.05, -0.86, 0.32 },  // FemurR1Height
    { -0.95, -0.92, 0.40 },  // FemurR1 POS X
    { -0.96, -0.93, 0.40 },  // FemurR1 POS Y
    { -0.97, -0.94, 0.40 },  // FemurR1 POS Z
    { -0.951, -0.921, 0.40 },// FemurR1 ANG X
    { -0.961, -0.931, 0.40 },// FemurR1 ANG Y
    { -0.971, -0.941, 0.40 },// FemurR1 ANG Z
    { -0.952, -0.922, 0.40 },// FemurR1 VEL X
    { -0.962, -0.932, 0.40 },// FemurR1 VEL Y
    { -0.972, -0.942, 0.40 },// FemurR1 VEL Z
    { -0.60, -0.72, 0.00 },  // FemurR1ImpRot

    { -1.07, -0.42, 0.32 },  // FemurR2Collision
    { -1.10, -0.22, 0.32 },  // FemurR2Height
    { -1.03, -0.31, 0.40 },  // FemurR2 POS X
    { -1.04, -0.32, 0.40 },  // FemurR2 POS Y
    { -1.05, -0.33, 0.40 },  // FemurR2 POS Z
    { -1.031, -0.311, 0.40 },// FemurR2 ANG X
    { -1.041, -0.321, 0.40 },// FemurR2 ANG Y
    { -1.051, -0.331, 0.40 },// FemurR2 ANG Z
    { -1.032, -0.312, 0.40 },// FemurR2 VEL X
    { -1.042, -0.322, 0.40 },// FemurR2 VEL Y
    { -1.052, -0.332, 0.40 },// FemurR2 VEL Z
    { -0.63, -0.24, 0.00 },  // FemurR2Rot

    { -1.10, 0.28, 0.32 },   // FemurR3ImpAngX
    { -1.05, 0.47, 0.32 },   // FemurR3ImpAngY
    { -1.02, 0.36, 0.40 },   // FemurR3ImpAngZ POS X
    { -1.03, 0.37, 0.40 },   // FemurR3ImpAngZ POS Y
    { -1.04, 0.38, 0.40 },   // FemurR3ImpAngZ POS Z
    { -1.021, 0.361, 0.40 },   // FemurR3ImpAngZ ANG X
    { -1.031, 0.371, 0.40 },   // FemurR3ImpAngZ ANG Y
    { -1.041, 0.381, 0.40 },   // FemurR3ImpAngZ ANG Z
    { -1.022, 0.362, 0.40 },   // FemurR3ImpAngZ VEL X
    { -1.032, 0.372, 0.40 },   // FemurR3ImpAngZ VEL Y
    { -1.042, 0.382, 0.40 },   // FemurR3ImpAngZ VEL Z
    { -0.63, 0.26, 0.00 },   // FemurR3ImpRot

    { -1.00, 0.97, 0.32 },   // FemurR4ImpAngX
    { -0.87, 1.12, 0.32 },   // FemurR4ImpAngY
    { -0.89, 1.01, 0.40 },   // FemurR4ImpAngZ POS X
    { -0.90, 1.02, 0.40 },   // FemurR4ImpAngZ POS Y
    { -0.91, 1.03, 0.40 },   // FemurR4ImpAngZ POS Z
    { -0.891, 1.011, 0.40 },   // FemurR4ImpAngZ ANG X
    { -0.901, 1.021, 0.40 },   // FemurR4ImpAngZ ANG Y
    { -0.911, 1.031, 0.40 },   // FemurR4ImpAngZ ANG Z
    { -0.892, 1.012, 0.40 },   // FemurR4ImpAngZ VEL X
    { -0.902, 1.022, 0.40 },   // FemurR4ImpAngZ VEL Y
    { -0.912, 1.032, 0.40 },   // FemurR4ImpAngZ VEL Z
    { -0.58, 0.75, 0.00 },   // FemurR4ImpRot

    /* { -0.100000, 0.722260, 0.032758 },   // HipImpAngX */
    /* { 0.100000, 0.722260, 0.032758 },    // HipImpAngY */
    /* { 0.000000, 0.696378, 0.129351 },    // HipImpAngZ */
    /* { 0.000000, 0.600004, 0.000000 },    // HipImpRot */
    /* { -0.100000, -0.709850, 0.109850 },  // NeckImpAngX */
    /* { 0.100000, -0.709850, 0.109850 },   // NeckImpAngY */
    /* { -0.000000, -0.639139, 0.180561 },  // NeckImpAngZ */
    /* { 0.000000, -0.599997, -0.000003 },  // NeckImpRot */
    { 1.70, -1.46, 0.57 },   // PatellaL1ImpAngX
    { 1.80, -1.29, 0.57 },   // PatellaL1ImpAngY
    { 1.76, -1.38, 0.67 },   // PatellaL1ImpAngZ POS X
    { 1.77, -1.39, 0.67 },   // PatellaL1ImpAngZ POS Y
    { 1.78, -1.40, 0.67 },   // PatellaL1ImpAngZ POS Z
    { 1.761, -1.381, 0.67 }, // PatellaL1ImpAngZ ANG X
    { 1.771, -1.391, 0.67 }, // PatellaL1ImpAngZ ANG Y
    { 1.781, -1.401, 0.67 }, // PatellaL1ImpAngZ ANG Z
    { 1.762, -1.382, 0.67 },   // PatellaL1ImpAngZ VEL X
    { 1.772, -1.392, 0.67 },   // PatellaL1ImpAngZ VEL Y
    { 1.782, -1.402, 0.67 },   // PatellaL1ImpAngZ VEL Z
    { 1.40, -1.18, 0.64 },   // PatellaL1ImpRot

    { 1.91, -0.56, 0.57 },   // PatellaL2ImpAngX
    { 1.95, -0.37, 0.57 },   // PatellaL2ImpAngY
    { 1.95, -0.47, 0.67 },   // PatellaL2ImpAngZ POS X
    { 1.96, -0.48, 0.67 },   // PatellaL2ImpAngZ POS Y
    { 1.97, -0.49, 0.67 },   // PatellaL2ImpAngZ POS Z
    { 1.951, -0.471, 0.67 },   // PatellaL2ImpAngZ ANG X
    { 1.961, -0.481, 0.67 },   // PatellaL2ImpAngZ ANG Y
    { 1.971, -0.491, 0.67 },   // PatellaL2ImpAngZ ANG Z
    { 1.952, -0.472, 0.67 },   // PatellaL2ImpAngZ VEL X
    { 1.962, -0.482, 0.67 },   // PatellaL2ImpAngZ VEL Y
    { 1.972, -0.492, 0.67 },   // PatellaL2ImpAngZ VEL Z
    { 1.54, -0.40, 0.64 },   // PatellaL2ImpRot

    { 1.93, 0.50, 0.57 },    // PatellaL3ImpAngX
    { 1.88, 0.70, 0.57 },    // PatellaL3ImpAngY
    { 1.92, 0.60, 0.67 },    // PatellaL3ImpAngZ POS X
    { 1.93, 0.61, 0.67 },    // PatellaL3ImpAngZ POS Y
    { 1.94, 0.62, 0.67 },    // PatellaL3ImpAngZ POS Z
    { 1.921, 0.601, 0.67 },    // PatellaL3ImpAngZ ANG X
    { 1.931, 0.611, 0.67 },    // PatellaL3ImpAngZ ANG Y
    { 1.941, 0.621, 0.67 },    // PatellaL3ImpAngZ ANG Z
    { 1.922, 0.602, 0.67 },    // PatellaL3ImpAngZ VEL X
    { 1.932, 0.612, 0.67 },    // PatellaL3ImpAngZ VEL Y
    { 1.942, 0.622, 0.67 },    // PatellaL3ImpAngZ VEL Z
    { 1.52, 0.50, 0.64 },    // PatellaL3ImpRot

    { 1.65, 1.52, 0.57 },    // PatellaL4ImpAngX
    { 1.53, 1.67, 0.57 },    // PatellaL4ImpAngY
    { 1.60, 1.61, 0.67 },    // PatellaL4ImpAngZ POS X
    { 1.61, 1.62, 0.67 },    // PatellaL4ImpAngZ POS Y
    { 1.62, 1.63, 0.67 },    // PatellaL4ImpAngZ POS Z
    { 1.601, 1.611, 0.67 },    // PatellaL4ImpAngZ ANG X
    { 1.611, 1.621, 0.67 },    // PatellaL4ImpAngZ ANG Y
    { 1.621, 1.631, 0.67 },    // PatellaL4ImpAngZ ANG Z
    { 1.602, 1.612, 0.67 },    // PatellaL4ImpAngZ VEL X
    { 1.612, 1.622, 0.67 },    // PatellaL4ImpAngZ VEL Y
    { 1.622, 1.632, 0.67 },    // PatellaL4ImpAngZ VEL Z
    { 1.29, 1.34, 0.64 },    // PatellaL4ImpRot

    { -1.70, -1.46, 0.57 },  // PatellaR1ImpAngX
    { -1.80, -1.29, 0.57 },  // PatellaR1ImpAngY
    { -1.76, -1.38, 0.67 },  // PatellaR1ImpAngZ POS X
    { -1.77, -1.39, 0.67 },  // PatellaR1ImpAngZ POS Y
    { -1.78, -1.40, 0.67 },  // PatellaR1ImpAngZ POS Z
    { -1.761, -1.381, 0.67 },  // PatellaR1ImpAngZ ANG X
    { -1.771, -1.391, 0.67 },  // PatellaR1ImpAngZ ANG Y
    { -1.781, -1.401, 0.67 },  // PatellaR1ImpAngZ ANG Z
    { -1.762, -1.382, 0.67 },  // PatellaR1ImpAngZ VEL X
    { -1.772, -1.392, 0.67 },  // PatellaR1ImpAngZ VEL Y
    { -1.782, -1.402, 0.67 },  // PatellaR1ImpAngZ VEL Z
    { -1.40, -1.18, 0.64 },  // PatellaR1ImpRot

    { -1.91, -0.56, 0.57 },  // PatellaR2ImpAngX
    { -1.95, -0.37, 0.57 },  // PatellaR2ImpAngY
    { -1.95, -0.47, 0.67 },  // PatellaR2ImpAngZ POS X
    { -1.96, -0.48, 0.67 },  // PatellaR2ImpAngZ POS Y
    { -1.97, -0.49, 0.67 },  // PatellaR2ImpAngZ POS Z
    { -1.951, -0.471, 0.67 },  // PatellaR2ImpAngZ ANG X
    { -1.961, -0.481, 0.67 },  // PatellaR2ImpAngZ ANG Y
    { -1.971, -0.491, 0.67 },  // PatellaR2ImpAngZ ANG Z
    { -1.952, -0.472, 0.67 },  // PatellaR2ImpAngZ VEL X
    { -1.962, -0.482, 0.67 },  // PatellaR2ImpAngZ VEL Y
    { -1.972, -0.492, 0.67 },  // PatellaR2ImpAngZ VEL Z
    { -1.54, -0.40, 0.64 },  // PatellaR2ImpRot

    { -1.93, 0.50, 0.57 },   // PatellaR3ImpAngX
    { -1.88, 0.70, 0.57 },   // PatellaR3ImpAngY
    { -1.92, 0.60, 0.67 },   // PatellaR3ImpAngZ POS X
    { -1.93, 0.61, 0.67 },   // PatellaR3ImpAngZ POS Y
    { -1.94, 0.62, 0.67 },   // PatellaR3ImpAngZ POS Z
    { -1.921, 0.601, 0.67 },   // PatellaR3ImpAngZ ANG X
    { -1.931, 0.611, 0.67 },   // PatellaR3ImpAngZ ANG Y
    { -1.941, 0.621, 0.67 },   // PatellaR3ImpAngZ ANG Z
    { -1.922, 0.602, 0.67 },   // PatellaR3ImpAngZ VEL X
    { -1.932, 0.612, 0.67 },   // PatellaR3ImpAngZ VEL Y
    { -1.942, 0.622, 0.67 },   // PatellaR3ImpAngZ VEL Z
    { -1.52, 0.50, 0.64 },   // PatellaR3ImpRot

    { -1.65, 1.52, 0.57 },   // PatellaR4ImpAngX
    { -1.53, 1.67, 0.57 },   // PatellaR4ImpAngY
    { -1.60, 1.61, 0.67 },   // PatellaR4ImpAngZ POS X
    { -1.61, 1.62, 0.67 },   // PatellaR4ImpAngZ POS Y
    { -1.62, 1.63, 0.67 },   // PatellaR4ImpAngZ POS Z
    { -1.601, 1.611, 0.67 },   // PatellaR4ImpAngZ ANG X
    { -1.611, 1.621, 0.67 },   // PatellaR4ImpAngZ ANG Y
    { -1.621, 1.631, 0.67 },   // PatellaR4ImpAngZ ANG Z
    { -1.602, 1.612, 0.67 },   // PatellaR4ImpAngZ VEL X
    { -1.612, 1.622, 0.67 },   // PatellaR4ImpAngZ VEL Y
    { -1.622, 1.632, 0.67 },   // PatellaR4ImpAngZ VEL Z
    { -1.29, 1.34, 0.64 },   // PatellaR4ImpRot

    /* { -0.100000, 0.000000, 0.000000 },   // SternumImpAngX */
    /* { 0.100000, 0.000000, 0.000000 },    // SternumImpAngY */
    /* { 0.000000, 0.000000, 0.100000 },    // SternumImpAngZ */

    { 2.71, -2.05, -0.72 },  // TarsusL1ImpAngX
    { 2.81, -1.87, -0.72 },  // TarsusL1ImpAngY
    { 2.84, -2.01, -0.69 },  // TarsusL1ImpAngZ POS X
    { 2.85, -2.02, -0.69 },  // TarsusL1ImpAngZ POS Y
    { 2.86, -2.03, -0.69 },  // TarsusL1ImpAngZ POS Z
    { 2.841, -2.011, -0.69 },  // TarsusL1ImpAngZ ANG X
    { 2.851, -2.021, -0.69 },  // TarsusL1ImpAngZ ANG Y
    { 2.861, -2.031, -0.69 },  // TarsusL1ImpAngZ ANG Z
    { 2.842, -2.012, -0.69 },  // TarsusL1ImpAngZ VEL X
    { 2.852, -2.022, -0.69 },  // TarsusL1ImpAngZ VEL Y
    { 2.862, -2.032, -0.69 },  // TarsusL1ImpAngZ VEL Z
    { 2.70, -1.93, -0.47 },  // TarsusL1ImpRot

    { 3.07, -0.74, -0.72 },  // TarsusL2ImpAngX
    { 3.10, -0.52, -0.72 },  // TarsusL2ImpAngY
    { 3.18, -0.66, -0.69 },  // TarsusL2ImpAngZ POS X
    { 3.19, -0.76, -0.69 },  // TarsusL2ImpAngZ POS Y
    { 3.20, -0.76, -0.69 },  // TarsusL2ImpAngZ POS Z
    { 3.181, -0.661, -0.69 },  // TarsusL2ImpAngZ ANG X
    { 3.191, -0.761, -0.69 },  // TarsusL2ImpAngZ ANG Y
    { 3.201, -0.761, -0.69 },  // TarsusL2ImpAngZ ANG Z
    { 3.182, -0.662, -0.69 },  // TarsusL2ImpAngZ VEL X
    { 3.192, -0.762, -0.69 },  // TarsusL2ImpAngZ VEL Y
    { 3.202, -0.762, -0.69 },  // TarsusL2ImpAngZ VEL Z
    { 3.02, -0.61, -0.47 },  // TarsusL2ImpRot

    { 3.06, 0.87, -0.72 },   // TarsusL3ImpAngX
    { 3.01, 1.02, -0.72 },   // TarsusL3ImpAngY
    { 3.13, 0.90, -0.69 },   // TarsusL3ImpAngZ POS X
    { 3.14, 0.90, -0.69 },   // TarsusL3ImpAngZ POS Y
    { 3.15, 0.90, -0.69 },   // TarsusL3ImpAngZ POS Z
    { 3.131, 0.901, -0.69 },   // TarsusL3ImpAngZ ANG X
    { 3.141, 0.901, -0.69 },   // TarsusL3ImpAngZ ANG Y
    { 3.151, 0.901, -0.69 },   // TarsusL3ImpAngZ ANG Z
    { 3.132, 0.902, -0.69 },   // TarsusL3ImpAngZ VEL X
    { 3.142, 0.902, -0.69 },   // TarsusL3ImpAngZ VEL Y
    { 3.152, 0.902, -0.69 },   // TarsusL3ImpAngZ VEL Z
    { 2.97, 0.88, -0.47 },   // TarsusL3ImpRot

    { 2.55, 2.27, -0.72 },   // TarsusL4ImpAngX
    { 2.42, 2.43, -0.72 },   // TarsusL4ImpAngY
    { 2.56, 2.41, -0.69 },   // TarsusL4ImpAngZ POS X
    { 2.57, 2.42, -0.69 },   // TarsusL4ImpAngZ POS Y
    { 2.58, 2.43, -0.69 },   // TarsusL4ImpAngZ POS Z
    { 2.561, 2.411, -0.69 },   // TarsusL4ImpAngZ ANG X
    { 2.571, 2.421, -0.69 },   // TarsusL4ImpAngZ ANG Y
    { 2.581, 2.431, -0.69 },   // TarsusL4ImpAngZ ANG Z
    { 2.562, 2.412, -0.69 },   // TarsusL4ImpAngZ VEL X
    { 2.572, 2.422, -0.69 },   // TarsusL4ImpAngZ VEL Y
    { 2.582, 2.432, -0.69 },   // TarsusL4ImpAngZ VEL Z
    { 2.44, 2.31, -0.47 },   // TarsusL4ImpRot

    { -2.71, -2.05, -0.72 }, // TarsusR1ImpAngX
    { -2.81, -1.87, -0.72 }, // TarsusR1ImpAngY
    { -2.84, -2.01, -0.69 }, // TarsusR1ImpAngZ POS X
    { -2.85, -2.02, -0.69 }, // TarsusR1ImpAngZ POS Y
    { -2.86, -2.03, -0.69 }, // TarsusR1ImpAngZ POS Z
    { -2.841, -2.011, -0.69 }, // TarsusR1ImpAngZ ANG X
    { -2.851, -2.021, -0.69 }, // TarsusR1ImpAngZ ANG Y
    { -2.861, -2.031, -0.69 }, // TarsusR1ImpAngZ ANG Z
    { -2.842, -2.012, -0.69 }, // TarsusR1ImpAngZ VEL X
    { -2.852, -2.022, -0.69 }, // TarsusR1ImpAngZ VEL Y
    { -2.862, -2.032, -0.69 }, // TarsusR1ImpAngZ VEL Z
    { -2.70, -1.93, -0.47 }, // TarsusR1ImpRot

    { -3.07, -0.77, -0.72 }, // TarsusR2ImpAngX
    { -3.10, -0.57, -0.72 }, // TarsusR2ImpAngY
    { -3.18, -0.69, -0.69 }, // TarsusR2ImpAngZ POS X
    { -3.19, -0.70, -0.69 }, // TarsusR2ImpAngZ POS Y
    { -3.20, -0.71, -0.69 }, // TarsusR2ImpAngZ POS Z
    { -3.181, -0.691, -0.69 }, // TarsusR2ImpAngZ ANG X
    { -3.191, -0.701, -0.69 }, // TarsusR2ImpAngZ ANG Y
    { -3.201, -0.711, -0.69 }, // TarsusR2ImpAngZ ANG Z
    { -3.182, -0.692, -0.69 }, // TarsusR2ImpAngZ VEL X
    { -3.192, -0.702, -0.69 }, // TarsusR2ImpAngZ VEL Y
    { -3.202, -0.712, -0.69 }, // TarsusR2ImpAngZ VEL Z
    { -3.02, -0.66, -0.47 }, // TarsusR2ImpRot

    { -3.06, 0.81, -0.72 },  // TarsusR3ImpAngX
    { -3.01, 1.00, -0.72 },  // TarsusR3ImpAngY
    { -3.13, 0.93, -0.69 },  // TarsusR3ImpAngZ POS X
    { -3.14, 0.94, -0.69 },  // TarsusR3ImpAngZ POS Y
    { -3.15, 0.95, -0.69 },  // TarsusR3ImpAngZ POS Z
    { -3.131, 0.931, -0.69 },  // TarsusR3ImpAngZ ANG X
    { -3.141, 0.941, -0.69 },  // TarsusR3ImpAngZ ANG Y
    { -3.151, 0.951, -0.69 },  // TarsusR3ImpAngZ ANG Z
    { -3.132, 0.932, -0.69 },  // TarsusR3ImpAngZ VEL X
    { -3.142, 0.942, -0.69 },  // TarsusR3ImpAngZ VEL Y
    { -3.152, 0.952, -0.69 },  // TarsusR3ImpAngZ VEL Z
    { -2.97, 0.88, -0.47 },  // TarsusR3ImpRot

    { -2.55, 2.27, -0.72 },  // TarsusR4ImpAngX
    { -2.42, 2.43, -0.72 },  // TarsusR4ImpAngY
    { -2.56, 2.41, -0.69 },  // TarsusR4ImpAngZ POS X
    { -2.57, 2.42, -0.69 },  // TarsusR4ImpAngZ POS Y
    { -2.58, 2.43, -0.69 },  // TarsusR4ImpAngZ POS Z
    { -2.561, 2.411, -0.69 },  // TarsusR4ImpAngZ ANG X
    { -2.571, 2.421, -0.69 },  // TarsusR4ImpAngZ ANG Y
    { -2.581, 2.431, -0.69 },  // TarsusR4ImpAngZ ANG Z
    { -2.562, 2.412, -0.69 },  // TarsusR4ImpAngZ VEL X
    { -2.572, 2.422, -0.69 },  // TarsusR4ImpAngZ VEL Y
    { -2.582, 2.432, -0.69 },  // TarsusR4ImpAngZ VEL Z
    { -2.44, 2.31, -0.47 },  // TarsusR4ImpRot

    { 2.33, -1.83, 0.02 },   // TibiaL1ImpAngX
    { 2.43, -1.66, 0.02 },   // TibiaL1ImpAngY
    { 2.45, -1.78, 0.07 },   // TibiaL1ImpAngZ POS X
    { 2.46, -1.79, 0.07 },   // TibiaL1ImpAngZ POS Y
    { 2.47, -1.80, 0.07 },   // TibiaL1ImpAngZ POS Z
    { 2.451, -1.781, 0.07 },   // TibiaL1ImpAngZ ANG X
    { 2.461, -1.791, 0.07 },   // TibiaL1ImpAngZ ANG Y
    { 2.471, -1.801, 0.07 },   // TibiaL1ImpAngZ ANG Z
    { 2.452, -1.782, 0.07 },   // TibiaL1ImpAngZ VEL X
    { 2.462, -1.792, 0.07 },   // TibiaL1ImpAngZ VEL Y
    { 2.472, -1.802, 0.07 },   // TibiaL1ImpAngZ VEL Z
    { 2.09, -1.57, 0.50 },   // TibiaL1ImpRot

    { 2.64, -0.69, 0.02 },   // TibiaL2ImpAngX
    { 2.67, -0.50, 0.02 },   // TibiaL2ImpAngY
    { 2.74, -0.61, 0.07 },   // TibiaL2ImpAngZ POS X
    { 2.75, -0.62, 0.07 },   // TibiaL2ImpAngZ POS Y
    { 2.76, -0.63, 0.07 },   // TibiaL2ImpAngZ POS Z
    { 2.741, -0.611, 0.07 },   // TibiaL2ImpAngZ ANG X
    { 2.751, -0.621, 0.07 },   // TibiaL2ImpAngZ ANG Y
    { 2.761, -0.631, 0.07 },   // TibiaL2ImpAngZ ANG Z
    { 2.742, -0.612, 0.07 },   // TibiaL2ImpAngZ VEL X
    { 2.752, -0.622, 0.07 },   // TibiaL2ImpAngZ VEL Y
    { 2.762, -0.632, 0.07 },   // TibiaL2ImpAngZ VEL Z
    { 2.32, -0.53, 0.50 },   // TibiaL2ImpRot

    { 2.64, 0.69, 0.02 },    // TibiaL3ImpAngX
    { 2.59, 0.89, 0.02 },    // TibiaL3ImpAngY
    { 2.69, 0.81, 0.07 },    // TibiaL3ImpAngZ POS X
    { 2.70, 0.82, 0.07 },    // TibiaL3ImpAngZ POS Y
    { 2.71, 0.83, 0.07 },    // TibiaL3ImpAngZ POS Z
    { 2.691, 0.811, 0.07 },    // TibiaL3ImpAngZ ANG X
    { 2.701, 0.821, 0.07 },    // TibiaL3ImpAngZ ANG Y
    { 2.711, 0.831, 0.07 },    // TibiaL3ImpAngZ ANG Z
    { 2.692, 0.812, 0.07 },    // TibiaL3ImpAngZ VEL X
    { 2.702, 0.822, 0.07 },    // TibiaL3ImpAngZ VEL Y
    { 2.712, 0.832, 0.07 },    // TibiaL3ImpAngZ VEL Z
    { 2.28, 0.70, 0.50 },    // TibiaL3ImpRot

    { 2.22, 1.99, 0.02 },    // TibiaL4ImpAngX
    { 2.09, 2.15, 0.02 },    // TibiaL4ImpAngY
    { 2.22, 2.12, 0.07 },    // TibiaL4ImpAngZ POS X
    { 2.23, 2.13, 0.07 },    // TibiaL4ImpAngZ POS Y
    { 2.24, 2.14, 0.07 },    // TibiaL4ImpAngZ POS Z
    { 2.221, 2.121, 0.07 },    // TibiaL4ImpAngZ ANG X
    { 2.231, 2.131, 0.07 },    // TibiaL4ImpAngZ ANG Y
    { 2.241, 2.141, 0.07 },    // TibiaL4ImpAngZ ANG Z
    { 2.222, 2.122, 0.07 },    // TibiaL4ImpAngZ VEL X
    { 2.232, 2.132, 0.07 },    // TibiaL4ImpAngZ VEL Y
    { 2.242, 2.142, 0.07 },    // TibiaL4ImpAngZ VEL Z
    { 1.89, 1.85, 0.50 },    // TibiaL4ImpRot

    { -2.33, -1.83, 0.02 },  // TibiaR1ImpAngX
    { -2.43, -1.66, 0.02 },  // TibiaR1ImpAngY
    { -2.45, -1.78, 0.07 },  // TibiaR1ImpAngZ POS X
    { -2.46, -1.79, 0.07 },  // TibiaR1ImpAngZ POS Y
    { -2.47, -1.80, 0.07 },  // TibiaR1ImpAngZ POS Z
    { -2.451, -1.781, 0.07 },  // TibiaR1ImpAngZ ANG X
    { -2.461, -1.791, 0.07 },  // TibiaR1ImpAngZ ANG Y
    { -2.471, -1.801, 0.07 },  // TibiaR1ImpAngZ ANG Z
    { -2.452, -1.782, 0.07 },  // TibiaR1ImpAngZ VEL X
    { -2.462, -1.792, 0.07 },  // TibiaR1ImpAngZ VEL Y
    { -2.472, -1.802, 0.07 },  // TibiaR1ImpAngZ VEL Z
    { -2.09, -1.57, 0.50 },  // TibiaR1ImpRot

    { -2.64, -0.69, 0.02 },  // TibiaR2ImpAngX
    { -2.67, -0.50, 0.02 },  // TibiaR2ImpAngY
    { -2.74, -0.61, 0.07 },  // TibiaR2ImpAngZ POS X
    { -2.75, -0.62, 0.07 },  // TibiaR2ImpAngZ POS Y
    { -2.76, -0.63, 0.07 },  // TibiaR2ImpAngZ POS Z
    { -2.741, -0.611, 0.07 },  // TibiaR2ImpAngZ ANG X
    { -2.751, -0.621, 0.07 },  // TibiaR2ImpAngZ ANG Y
    { -2.761, -0.631, 0.07 },  // TibiaR2ImpAngZ ANG Z
    { -2.742, -0.612, 0.07 },  // TibiaR2ImpAngZ VEL X
    { -2.752, -0.622, 0.07 },  // TibiaR2ImpAngZ VEL Y
    { -2.762, -0.632, 0.07 },  // TibiaR2ImpAngZ VEL Z
    { -2.32, -0.53, 0.50 },  // TibiaR2ImpRot

    { -2.64, 0.69, 0.02 },   // TibiaR3ImpAngX
    { -2.59, 0.89, 0.02 },   // TibiaR3ImpAngY
    { -2.69, 0.81, 0.07 },   // TibiaR3ImpAngZ POS X
    { -2.70, 0.82, 0.07 },   // TibiaR3ImpAngZ POS Y
    { -2.71, 0.83, 0.07 },   // TibiaR3ImpAngZ POS Z
    { -2.691, 0.811, 0.07 },   // TibiaR3ImpAngZ ANG X
    { -2.701, 0.821, 0.07 },   // TibiaR3ImpAngZ ANG Y
    { -2.711, 0.831, 0.07 },   // TibiaR3ImpAngZ ANG Z
    { -2.692, 0.812, 0.07 },   // TibiaR3ImpAngZ VEL X
    { -2.702, 0.822, 0.07 },   // TibiaR3ImpAngZ VEL Y
    { -2.712, 0.832, 0.07 },   // TibiaR3ImpAngZ VEL Z
    { -2.28, 0.70, 0.50 },   // TibiaR3ImpRot

    { -2.22, 1.99, 0.02 },   // TibiaR4ImpAngX
    { -2.09, 2.15, 0.02 },   // TibiaR4ImpAngY
    { -2.22, 2.12, 0.07 },   // TibiaR4ImpAngZ POS X
    { -2.23, 2.13, 0.07 },   // TibiaR4ImpAngZ POS Y
    { -2.24, 2.14, 0.07 },   // TibiaR4ImpAngZ POS Z
    { -2.221, 2.121, 0.07 },   // TibiaR4ImpAngZ ANG X
    { -2.231, 2.131, 0.07 },   // TibiaR4ImpAngZ ANG Y
    { -2.241, 2.141, 0.07 },   // TibiaR4ImpAngZ ANG Z
    { -2.222, 2.122, 0.07 },   // TibiaR4ImpAngZ VEL X
    { -2.232, 2.132, 0.07 },   // TibiaR4ImpAngZ VEL Y
    { -2.242, 2.142, 0.07 },   // TibiaR4ImpAngZ VEL Z
    { -1.89, 1.85, 0.50 },   // TibiaR4ImpRot

    { 0.45, -0.74, -0.00 },  // TrochanterL1ImpAngX
    { 0.55, -0.57, -0.00 },  // TrochanterL1ImpAngY
    { 0.50, -0.66,  0.10 },   // TrochanterL1ImpAngZ POS X
    { 0.51, -0.67,  0.10 },   // TrochanterL1ImpAngZ POS Y
    { 0.52, -0.68,  0.10 },   // TrochanterL1ImpAngZ POS Z
    { 0.501, -0.661,  0.10 },   // TrochanterL1ImpAngZ ANG X
    { 0.511, -0.671,  0.10 },   // TrochanterL1ImpAngZ ANG Y
    { 0.521, -0.681,  0.10 },   // TrochanterL1ImpAngZ ANG Z
    { 0.502, -0.662,  0.10 },   // TrochanterL1ImpAngZ VEL X
    { 0.512, -0.672,  0.10 },   // TrochanterL1ImpAngZ VEL Y
    { 0.522, -0.682,  0.10 },   // TrochanterL1ImpAngZ VEL Z
    { 0.40, -0.60, -0.00 },  // TrochanterL1ImpRot

    { 0.50, -0.31, 0.00 },   // TrochanterL2ImpAngX
    { 0.53, -0.12, 0.00 },   // TrochanterL2ImpAngY
    { 0.51, -0.22, 0.10 },   // TrochanterL2ImpAngZ POS X
    { 0.52, -0.23, 0.10 },   // TrochanterL2ImpAngZ POS Y
    { 0.53, -0.24, 0.10 },   // TrochanterL2ImpAngZ POS Z
    { 0.511, -0.221, 0.10 },   // TrochanterL2ImpAngZ ANG X
    { 0.521, -0.231, 0.10 },   // TrochanterL2ImpAngZ ANG Y
    { 0.531, -0.241, 0.10 },   // TrochanterL2ImpAngZ ANG Z
    { 0.512, -0.222, 0.10 },   // TrochanterL2ImpAngZ VEL X
    { 0.522, -0.232, 0.10 },   // TrochanterL2ImpAngZ VEL Y
    { 0.532, -0.242, 0.10 },   // TrochanterL2ImpAngZ VEL Z
    { 0.40, -0.20, 0.00 },   // TrochanterL2ImpRot

    { 0.54, 0.13, 0.00 },    // TrochanterL3ImpAngX
    { 0.49, 0.32, 0.00 },    // TrochanterL3ImpAngY
    { 0.51, 0.23, 0.10 },    // TrochanterL3ImpAngZ POS X
    { 0.52, 0.24, 0.10 },    // TrochanterL3ImpAngZ POS Y
    { 0.53, 0.25, 0.10 },    // TrochanterL3ImpAngZ POS Z
    { 0.511, 0.231, 0.10 },    // TrochanterL3ImpAngZ ANG X
    { 0.521, 0.241, 0.10 },    // TrochanterL3ImpAngZ ANG Y
    { 0.531, 0.251, 0.10 },    // TrochanterL3ImpAngZ ANG Z
    { 0.512, 0.232, 0.10 },    // TrochanterL3ImpAngZ VEL X
    { 0.522, 0.242, 0.10 },    // TrochanterL3ImpAngZ VEL Y
    { 0.532, 0.252, 0.10 },    // TrochanterL3ImpAngZ VEL Z
    { 0.40, 0.20, 0.00 },    // TrochanterL3ImpRot

    { 0.55, 0.60,  0.00 },   // TrochanterL4ImpAngX
    { 0.42, 0.75,  0.00 },   // TrochanterL4ImpAngY
    { 0.49, 0.67,  0.10 },    // TrochanterL4ImpAngZ POS X
    { 0.50, 0.68,  0.10 },    // TrochanterL4ImpAngZ POS Y
    { 0.51, 0.69,  0.10 },    // TrochanterL4ImpAngZ POS Z
    { 0.491, 0.671,  0.10 },    // TrochanterL4ImpAngZ ANG X
    { 0.501, 0.681,  0.10 },    // TrochanterL4ImpAngZ ANG Y
    { 0.511, 0.691,  0.10 },    // TrochanterL4ImpAngZ ANG Z
    { 0.492, 0.672,  0.10 },    // TrochanterL4ImpAngZ VEL X
    { 0.502, 0.682,  0.10 },    // TrochanterL4ImpAngZ VEL Y
    { 0.512, 0.692,  0.10 },    // TrochanterL4ImpAngZ VEL Z
    { 0.40, 0.60, -0.00 },   // TrochanterL4ImpRot

    { -0.45, -0.74, 0.00 },  // TrochanterR1ImpAngX
    { -0.55, -0.57, 0.00 },  // TrochanterR1ImpAngY
    { -0.50, -0.66, 0.10 },  // TrochanterR1ImpAngZ POS X
    { -0.51, -0.67, 0.10 },  // TrochanterR1ImpAngZ POS Y
    { -0.52, -0.68, 0.10 },  // TrochanterR1ImpAngZ POS Z
    { -0.501, -0.661, 0.10 },  // TrochanterR1ImpAngZ ANG X
    { -0.511, -0.671, 0.10 },  // TrochanterR1ImpAngZ ANG Y
    { -0.521, -0.681, 0.10 },  // TrochanterR1ImpAngZ ANG Z
    { -0.502, -0.662, 0.10 },  // TrochanterR1ImpAngZ VEL X
    { -0.512, -0.672, 0.10 },  // TrochanterR1ImpAngZ VEL Y
    { -0.522, -0.682, 0.10 },  // TrochanterR1ImpAngZ VEL Z
    { -0.40, -0.60, 0.00 },  // TrochanterR1ImpRot

    { -0.50, -0.31, 0.00 },   // TrochanterR2Collision
    { -0.53, -0.12, 0.00 },   // TrochanterR2Height
    { -0.51, -0.22, 0.10 },   // TrochanterR2 POS X
    { -0.52, -0.23, 0.10 },   // TrochanterR2 POS Y
    { -0.53, -0.24, 0.10 },   // TrochanterR2 POS Z
    { -0.511, -0.221, 0.10 }, // TrochanterR2 ANG X
    { -0.521, -0.231, 0.10 }, // TrochanterR2 ANG Y
    { -0.531, -0.241, 0.10 }, // TrochanterR2 ANG Z
    { -0.512, -0.222, 0.10 }, // TrochanterR2 VEL X
    { -0.522, -0.232, 0.10 }, // TrochanterR2 VEL Y
    { -0.532, -0.242, 0.10 }, // TrochanterR2 VEL Z
    { -0.40, -0.20, 0.00 },   // TrochanterR2Rot

    { -0.54, 0.13, 0.00},   // TrochanterR3Collision
    { -0.49, 0.32, 0.00},   // TrochanterR3Height
    { -0.51, 0.23, 0.10},   // TrochanterR3 POS X
    { -0.52, 0.24, 0.10},   // TrochanterR3 POS Y
    { -0.53, 0.25, 0.10},   // TrochanterR3 POS Z
    { -0.511, 0.231, 0.10}, // TrochanterR3 ANG X
    { -0.521, 0.241, 0.10}, // TrochanterR3 ANG Y
    { -0.531, 0.251, 0.10}, // TrochanterR3 ANG Z
    { -0.512, 0.232, 0.10}, // TrochanterR3 VEL X
    { -0.522, 0.242, 0.10}, // TrochanterR3 VEL Y
    { -0.532, 0.252, 0.10}, // TrochanterR3 VEL Z
    { -0.40, 0.20, 0.00},   // TrochanterR3Rot

    { -0.55, 0.60, 0.00 },   // TrochanterR4Collision
    { -0.42, 0.75, 0.00 },   // TrochanterR4Height
    { -0.49, 0.67, 0.10 },   // TrochanterR4 POS X
    { -0.50, 0.68, 0.10 },   // TrochanterR4 POS Y
    { -0.51, 0.69, 0.10 },   // TrochanterR4 POS Z
    { -0.491, 0.671, 0.10 }, // TrochanterR4 ANG X
    { -0.501, 0.681, 0.10 }, // TrochanterR4 ANG Y
    { -0.511, 0.691, 0.10 }, // TrochanterR4 ANG Z
    { -0.492, 0.672, 0.10 }, // TrochanterR4 VEL X
    { -0.502, 0.682, 0.10 }, // TrochanterR4 VEL Y
    { -0.512, 0.692, 0.10 }, // TrochanterR4 VEL Z
    { -0.40, 0.60, 0.00 },   // TrochanterR4Rot
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{
 // {  0.00,  0.89,  0.07 }, // Abdomin
 // {  0.00, -0.85,  0.24 }, // Eye
    {  0.60, -0.72,  0.00 }, // FemurL1
    {  0.63, -0.24,  0.00 }, // FemurL2
    {  0.63,  0.26,  0.00 }, // FemurL3
    {  0.58,  0.75,  0.00 }, // FemurL4
    { -0.60, -0.72,  0.00 }, // FemurR1
    { -0.63, -0.24,  0.00 }, // FemurR2
    { -0.63,  0.26,  0.00 }, // FemurR3
    { -0.58,  0.75,  0.00 }, // FemurR4
 // {  0.00,  0.60, 0.00  }, // Hip
 // {  0.00, -0.59, 0.00  }, // Neck
    {  1.40, -1.18,  0.64 }, // PatellaL1
    {  1.54, -0.40,  0.64 }, // PatellaL2
    {  1.52,  0.50,  0.64 }, // PatellaL3
    {  1.29,  1.34,  0.64 }, // PatellaL4
    { -1.40, -1.18,  0.64 }, // PatellaR1
    { -1.54, -0.40,  0.64 }, // PatellaR2
    { -1.52,  0.50,  0.64 }, // PatellaR3
    { -1.29,  1.34,  0.64 }, // PatellaR4
    {  2.70, -1.93, -0.47 }, // TarsusL1
    {  3.02, -0.66, -0.47 }, // TarsusL2
    {  2.97,  0.88, -0.47 }, // TarsusL3
    {  2.44,  2.31, -0.47 }, // TarsusL4
    { -2.70, -1.93, -0.47 }, // TarsusR1
    { -3.02, -0.66, -0.47 }, // TarsusR2
    { -2.97,  0.88, -0.47 }, // TarsusR3
    { -2.44,  2.31, -0.47 }, // TarsusR4
    {  2.09, -1.57,  0.50 }, // TibiaL1
    {  2.32, -0.53,  0.50 }, // TibiaL2
    {  2.28,  0.70,  0.50 }, // TibiaL3
    {  1.89,  1.85,  0.50 }, // TibiaL4
    { -2.09, -1.57,  0.50 }, // TibiaR1
    { -2.32, -0.53,  0.50 }, // TibiaR2
    { -2.28,  0.70,  0.50 }, // TibiaR3
    { -1.89,  1.85,  0.50 }, // TibiaR4
    {  0.40, -0.60,  0.00 }, // TrochanterL1
    {  0.40, -0.20,  0.00 }, // TrochanterL2
    {  0.40,  0.20,  0.00 }, // TrochanterL3
    {  0.40,  0.60,  0.00 }, // TrochanterL4
    { -0.40, -0.60,  0.00 }, // TrochanterR1
    { -0.40, -0.20,  0.00 }, // TrochanterR2
    { -0.40,  0.20,  0.00 }, // TrochanterR3
    { -0.40,  0.60,  0.00 }  // TrochanterR4
  };
  // clang-format on

  mSubstrate = new Substrate(inputs, hidden, outputs);

  // These variables are only used in HyperNEAT variations and not
  // in ESHyperNEAT.
  mSubstrate->m_allow_input_hidden_links  = false;
  mSubstrate->m_allow_input_output_links  = false;
  mSubstrate->m_allow_hidden_output_links = false;
  mSubstrate->m_allow_output_hidden_links = false;
  mSubstrate->m_allow_output_output_links = false;
  mSubstrate->m_allow_looped_hidden_links = false;
  mSubstrate->m_allow_looped_output_links = false;

  mSubstrate->m_allow_input_hidden_links  = true;
  mSubstrate->m_allow_input_output_links  = true;
  mSubstrate->m_allow_hidden_output_links = true;
  mSubstrate->m_allow_hidden_hidden_links = true;

  // These determine the output of the ESHyperNEAT CPPN hidden
  // and output nodes.
  //
  // The following activation functions are available:
  // NEAT::ActivationFunction::UNSIGNED_SIGMOID
  // NEAT::ActivationFunction::UNSIGNED_STEP
  // NEAT::ActivationFunction::UNSIGNED_SINE
  // NEAT::ActivationFunction::UNSIGNED_GAUSS
  // NEAT::ActivationFunction::SIGNED_SIGMOID
  // NEAT::ActivationFunction::SIGNED_STEP
  // NEAT::ActivationFunction::SIGNED_SINE
  // NEAT::ActivationFunction::SIGNED_GAUSS
  // NEAT::ActivationFunction::RELU
  // NEAT::ActivationFunction::LINEAR
  // NEAT::ActivationFunction::SOFTPLU
  // NEAT::ActivationFunction::TAHN
  // NEAT::ActivationFunction::TAHN_CUBIC
  // NEAT::ActivationFunction::ABS
  mSubstrate->m_hidden_nodes_activation = NEAT::ActivationFunction::SIGNED_SIGMOID;
  mSubstrate->m_output_nodes_activation = NEAT::ActivationFunction::UNSIGNED_SIGMOID;

  // This is only available in HyperNEAT and not ESHyperNEAT
  mSubstrate->m_with_distance = false;

  // When a new connection, it will not be added if the weight*maxWeightAndBias
  // is less than 0.2
  mSubstrate->m_max_weight_and_bias = 0.8;
}

/**
 * @brief
 *   This sets the default parameters for a population. By default, we mean
 *   that its set to what is defined in this function and not what is defined
 *   else where, for instance through loading a file
 */
void SpiderSwarm::setDefaultPopulation() {

  // sanity checking
  if (mSubstrate == nullptr)
    throw std::runtime_error("Substrate is not set");
  if (mPopulation != nullptr)
    delete mPopulation;

  NEAT::Parameters params;

  // Below follows all the parameters. The first comment is described
  // by the author of the library and "US" is a comment from our side to
  // explain more on the subject
  //
  // If there is a commented out value behind the value, then that represents
  // the default value for that parameter

  ////////////////////
  // Basic parameters
  ////////////////////

  // Size of population
  params.PopulationSize = 50;

  // If true, this enables dynamic compatibility thresholding
  // It will keep the number of species between MinSpecies and MaxSpecies
  params.DynamicCompatibility = true;

  // Minimum number of species
  params.MinSpecies = 5;

  // Maximum number of species
  params.MaxSpecies = 10;

  // Don't wipe the innovation database each generation?
  params.InnovationsForever = true;

  // Allow clones or nearly identical genomes to exist simultaneously in the population.
  // This is useful for non-deterministic environments,
  // as the same individual will get more than one chance to prove himself, also
  // there will be more chances the same individual to mutate in different ways.
  // The drawback is greatly increased time for reproduction. If you want to
  // search quickly, yet less efficient, leave this to true.
  params.AllowClones = true;

  ////////////////////////////////
  // GA Parameters
  ////////////////////////////////

  // Age treshold, meaning if a species is below it, it is considered young
  params.YoungAgeTreshold = 5;

  // Fitness boost multiplier for young species (1.0 means no boost)
  // Make sure it is >= 1.0 to avoid confusion
  params.YoungAgeFitnessBoost = 1.1;

  // Number of generations without improvement (stagnation) allowed for a species
  params.SpeciesMaxStagnation = 25;

  // Minimum jump in fitness necessary to be considered as improvement.
  // Setting this value to 0.0 makes the system to behave like regular NEAT.
  params.StagnationDelta = 0.0;

  // Age threshold, meaning if a species is above it, it is considered old
  params.OldAgeTreshold = 30;

  // Multiplier that penalizes old species.
  // Make sure it is <= 1.0 to avoid confusion.
  params.OldAgePenalty = 1.1;

  // Detect competetive coevolution stagnation
  // This kills the worst species of age >N (each X generations)
  params.DetectCompetetiveCoevolutionStagnation = false;

  // Each X generation..
  params.KillWorstSpeciesEach = 15;

  // Of age above..
  params.KillWorstAge = 10;

  // Percent of best individuals that are allowed to reproduce. 1.0 = 100%
  params.SurvivalRate = 0.25;

  // Probability for a baby to result from sexual reproduction (crossover/mating). 1.0 = 100%
  // If asexual reprodiction is chosen, the baby will be mutated 100%
  params.CrossoverRate = 0.7;

  // If a baby results from sexual reproduction, this probability determines if mutation will
  // be performed after crossover. 1.0 = 100% (always mutate after crossover)
  params.OverallMutationRate = 0.25;

  // Probability for a baby to result from inter-species mating.
  params.InterspeciesCrossoverRate = 0.0001;

  // Probability for a baby to result from Multipoint Crossover when mating. 1.0 = 100%
  // The default is the Average mating.
  params.MultipointCrossoverRate = 0.75;

  // Performing roulette wheel selection or not?
  params.RouletteWheelSelection = false;

  // For tournament selection
  params.TournamentSize = 4;

  // Fraction of individuals to be copied unchanged
  params.EliteFraction = 0.1; // 0.001

  ///////////////////////////////////
  // Structural Mutation parameters
  ///////////////////////////////////

  // Probability for a baby to be mutated with the Add-Neuron mutation.
  params.MutateAddNeuronProb = 0.08;

  // Allow splitting of any recurrent links
  params.SplitRecurrent = true;

  // Allow splitting of looped recurrent links
  params.SplitLoopedRecurrent = true;

  // Probability for a baby to be mutated with the Add-Link mutation
  params.MutateAddLinkProb = 0.06;

  // Probability for a new incoming link to be from the bias neuron;
  // This enforces it. A value of 0.0 doesn't mean there will not be such links
  params.MutateAddLinkFromBiasProb = 0.0;

  // Probability for a baby to be mutated with the Remove-Link mutation
  params.MutateRemLinkProb = 0.1;

  // Probability for a baby that a simple neuron will be replaced with a link
  params.MutateRemSimpleNeuronProb = 0.1;

  // Maximum number of tries to find 2 neurons to add/remove a link
  params.LinkTries = 32;

  // Probability that a link mutation will be made recurrent
  params.RecurrentProb = 0.25;

  // Probability that a recurrent link mutation will be looped
  params.RecurrentLoopProb = 0.25;

  ///////////////////////////////////
  // Parameter Mutation parameters
  ///////////////////////////////////

  // Probability for a baby's weights to be mutated
  params.MutateWeightsProb = 0.94;

  // Probability for a severe (shaking) weight mutation
  params.MutateWeightsSevereProb = 0.25;

  // Probability for a particular gene's weight to be mutated. 1.0 = 100%
  params.WeightMutationRate = 1.0;

  // Maximum perturbation for a weight mutation
  params.WeightMutationMaxPower = 1.0;

  // Maximum magnitude of a replaced weight
  params.WeightReplacementMaxPower = 1.0;

  // Maximum absolute magnitude of a weight
  // US: This only affects the weight in the CPPN and not the
  //     weights in the connection generated by the CPPN for the
  //     network we use.
  params.MaxWeight = 8.0;

  // Probability for a baby's A activation function parameters to be perturbed
  params.MutateActivationAProb = 0.1;

  // Probability for a baby's B activation function parameters to be perturbed
  params.MutateActivationBProb = 0.1;

  // Maximum magnitude for the A parameter perturbation
  params.ActivationAMutationMaxPower = 0.0;

  // Maximum magnitude for the B parameter perturbation
  params.ActivationBMutationMaxPower = 0.0;

  // Activation parameter A min/max
  params.MinActivationA = 1.0;
  params.MaxActivationA = 2.0;

  // Activation parameter B min/max
  params.MinActivationB = 0.0;
  params.MaxActivationB = 0.0;

  // Maximum magnitude for time costants perturbation
  params.TimeConstantMutationMaxPower = 0.0;

  // Maximum magnitude for biases perturbation
  params.BiasMutationMaxPower = params.WeightMutationMaxPower;

  // Probability for a baby's neuron time constant values to be mutated
  params.MutateNeuronTimeConstantsProb = 0.0;

  // Probability for a baby's neuron bias values to be mutated
  params.MutateNeuronBiasesProb = 0.0;

  // Time constant range
  params.MinNeuronTimeConstant = 0.0;
  params.MaxNeuronTimeConstant = 0.0;

  // Bias range
  params.MinNeuronBias = -params.MaxWeight; //0.0;
  params.MaxNeuronBias = params.MaxWeight; //0.0;

  // Probability for a baby that an activation function type will be changed for a single neuron
  // considered a structural mutation because of the large impact on fitness
  params.MutateNeuronActivationTypeProb = 0.15;

  // Probabilities for a particular activation function appearance
  params.ActivationFunction_SignedSigmoid_Prob = 1.0;
  params.ActivationFunction_UnsignedSigmoid_Prob = 1.0;
  params.ActivationFunction_Tanh_Prob = 1.0;
  params.ActivationFunction_TanhCubic_Prob = 1.0;
  params.ActivationFunction_SignedStep_Prob = 1.0;
  params.ActivationFunction_UnsignedStep_Prob = 1.0;
  params.ActivationFunction_SignedGauss_Prob = 1.0;
  params.ActivationFunction_UnsignedGauss_Prob = 1.0;
  params.ActivationFunction_Abs_Prob = 1.0;
  params.ActivationFunction_SignedSine_Prob = 1.0;
  params.ActivationFunction_UnsignedSine_Prob = 1.0;
  params.ActivationFunction_Linear_Prob = 0.0;
  params.ActivationFunction_Relu_Prob = 0.0;
  params.ActivationFunction_Softplus_Prob = 0.0;

  params.BiasMutationMaxPower = 0.5;

  // Genome properties parameters
  // params.DontUseBiasNeuron                       = false;
  // params.AllowLoops                              = true;

  /////////////////////////////////////
  // Speciation parameters
  /////////////////////////////////////

  // Percent of disjoint genes importance
  params.DisjointCoeff = 1.0;

  // Percent of excess genes importance
  params.ExcessCoeff = 1.0;

  // Average weight difference importance
  params.WeightDiffCoeff = 0.5;

  // Node-specific activation parameter A difference importance
  params.ActivationADiffCoeff = 0.0;

  // Node-specific activation parameter B difference importance
  params.ActivationBDiffCoeff = 0.0;

  // Average time constant difference importance
  params.TimeConstantDiffCoeff = 0.0;

  // Average bias difference importance
  params.BiasDiffCoeff = 0.0;

  // Activation function type difference importance
  params.ActivationFunctionDiffCoeff = 0.0;

  // Compatibility treshold
  params.CompatTreshold = 5.0;

  // Minumal value of the compatibility treshold
  params.MinCompatTreshold = 0.2;

  // Modifier per generation for keeping the species stable
  params.CompatTresholdModifier = 0.3;

  // Per how many generations to change the treshold
  // (used in generational mode)
  params.CompatTreshChangeInterval_Generations = 1;

  // Per how many evaluations to change the treshold
  // (used in steady state mode)
  params.CompatTreshChangeInterval_Evaluations = 10;

  // ES-HyperNEAT parameters

  // US: Determines the threshold that determines whether a
  // another node within the Quad Tree is to be added.
  // The sum of all weights of the children of a specific
  // quadpoint has to be higher than the DivisionThreshold.
  params.DivisionThreshold = 0.03; // 0.03

  // US: The variance threshold is used to determine whether
  // new connections for a specific QuadPoint is suppose be
  // generated. If the sum of all weights of a quadpoint is
  // higher than the variance threshold, it will generat new
  // connections for that point.
  params.VarianceThreshold = 0.05; // 0.05

  // Used for Band prunning.
  //
  // US: This determines whether a new connection is to be added or not.
  // If the output of the CPPN is higher than BandThreshold, add new
  // connections.
  params.BandThreshold = 0.28; // 0.3

  // Max and Min Depths of the quadtree
  //
  // US: Keep in mind that MaxDepth determines how many connections
  // may be generated. For any given N, (4^N) * ((4^N) - 1) / 2 connections
  // may be generated.
  params.InitialDepth = 3;
  params.MaxDepth = 4; // 3

  // How many hidden layers before connecting nodes to output. At 0 there is
  // one hidden layer. At 1, there are two and so on.
  params.IterationLevel = 1;

  // The Bias value for the CPPN queries.
  params.CPPN_Bias = 1.0;

  // Quadtree Dimensions
  // The range of the tree. Typically set to 2,
  params.Width = 3.0;
  params.Height = 3.0;

  // The (x, y) coordinates of the tree
  params.Qtree_X = 0.0;
  params.Qtree_Y = 0.0;

  // Use Link Expression output
  params.Leo = false;

  // Threshold above which a connection is expressed
  params.LeoThreshold = 0.1;

  // Use geometric seeding. Currently only along the X axis. 1
  params.LeoSeed = false;

  params.GeometrySeed = false;

  NEAT::Genome genome(0,
                      mSubstrate->GetMinCPPNInputs(),
                      0,
                      mSubstrate->GetMinCPPNOutputs(),
                      false,
                      NEAT::ActivationFunction::SIGNED_SIGMOID,
                      NEAT::ActivationFunction::SIGNED_SIGMOID,
                      0,
                      params);

  mPopulation = new NEAT::Population(genome, params, true, 1.0, time(0));
}
