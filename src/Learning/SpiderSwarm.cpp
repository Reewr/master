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
    , mIterationDuration(10)
    , mBestPossibleFitness(0)
    , mDrawDebugNetworks(false)
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
  std::string popFilename   = filename + ".population";
  std::string paramFilename = filename + ".parameters";
  std::string subFilename   = filename + ".substrate";

  mPopulation->Save(popFilename.c_str());
  mSubstrate->save(subFilename);
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
  std::string popFilename   = filename + ".population";
  std::string paramFilename = filename + ".parameters";
  std::string subFilename   = filename + ".substrate";

  if (mPopulation != nullptr)
    delete mPopulation;

  mPopulation = new NEAT::Population(popFilename.c_str());
  mSubstrate->load(subFilename);

  mCurrentBatch        = 0;
  mBestIndex           = 0;
  mGeneration          = mPopulation->m_Generation;
  mCurrentDuration     = 0;
  mBestPossibleFitness = 0;

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
    float  bestOfSpecies = 0;
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

      if (fitness > bestOfSpecies) {
        bestOfSpecies = fitness;
        leaderIndex   = index;
      }

      index += 1;
    }

    mSpeciesLeaders.push_back(leaderIndex);
  }

  ++mGeneration;
  mBestIndex           = bestIndex;
  mBestPossibleFitness = mmm::max(best, mBestPossibleFitness);

  mLog->info("Generation {}", mGeneration);
  mLog->info("Best of current generation {} %, Best of all: {} %",
             best * 100,
             mBestPossibleFitness * 100);

  for (auto i : mSpeciesLeaders) {
    mLog->info("-------------------------------------");
    mLog->info("The best in species: {}-{} >>= {} %{}",
               mPhenotypes[i].speciesIndex,
               mPhenotypes[i].individualIndex,
               mmm::product(mPhenotypes[i].fitness) * 100.f,
               i == mBestIndex ? " (best)" : "");

    size_t j = 0;
    for (auto& f : Phenotype::FITNESS_HANDLERS) {
      mLog->info("  {}: {} %", f.name(), mPhenotypes[i].fitness[j] * 100.f);
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

  size_t index      = 0;
  bool   addLeaders = mSpeciesLeaders.size() == 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    auto& species = mPopulation->m_Species[i];

    if (addLeaders) {
      mSpeciesLeaders.push_back(index);
    }

    for (size_t j = 0; j < species.m_Individuals.size(); ++j) {
      // Add new element if the population size has increased
      if (index >= mPhenotypes.size()) {
        mLog->debug("Adding new spider due to increase in population");
        mPhenotypes.push_back(Phenotype());
      }

      mPhenotypes[index].reset(i, j);
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
    { 2.841038, -2.009333, -1.048089 },  // L1Tip
    { 3.175848, -0.689457, -1.048092 },  // L2Tip
    { 3.122624, 0.929526, -1.048092 },   // L3Tip
    { 2.559226, 2.411806, -1.048092 },   // L4Tip
    { -2.841039, -2.009335, -1.048092 }, // R1Tip
    { -3.175846, -0.689457, -1.048092 }, // R2Tip
    { -3.122626, 0.929525, -1.048092 },  // R3Tip
    { -2.559227, 2.411805, -1.048092 },  // R4Tip
    { -0.100000, 1.565926, 0.258819 },   // AbdominImpAngX
    { 0.100000, 1.565926, 0.258819 },    // AbdominImpAngY
    { 0.000000, 1.540044, 0.355412 },    // AbdominImpAngZ
    { 0.000000, 0.898278, 0.079924 },    // AbdominImpRot
    { -0.100000, -0.850000, 0.446689 },  // EyeImpAngX
    { 0.100000, -0.850000, 0.446689 },   // EyeImpAngY
    { 0.000000, -0.850000, 0.546689 },   // EyeImpAngZ
    { 0.000000, -0.850000, 0.242041 },   // EyeImpRot
    { 0.958661, -1.038013, 0.324071 },   // FemurL1ImpAngX
    { 1.058661, -0.864808, 0.324071 },   // FemurL1ImpAngY
    { 0.958988, -0.922732, 0.405986 },   // FemurL1ImpAngZ
    { 0.607846, -0.720000, -0.000000 },  // FemurL1ImpRot
    { 1.074779, -0.420524, 0.324071 },   // FemurL2ImpAngX
    { 1.109508, -0.223563, 0.324071 },   // FemurL2ImpAngY
    { 1.035657, -0.312084, 0.405986 },   // FemurL2ImpAngZ
    { 0.636354, -0.241676, 0.000000 },   // FemurL2ImpRot
    { 1.104755, 0.285311, 0.324071 },    // FemurL3ImpAngX
    { 1.052991, 0.478496, 0.324071 },    // FemurL3ImpAngY
    { 1.023470, 0.367058, 0.405986 },    // FemurL3ImpAngZ
    { 0.631822, 0.262117, 0.000000 },    // FemurL3ImpRot
    { 1.002671, 0.975160, 0.324071 },    // FemurL4ImpAngX
    { 0.874113, 1.128369, 0.324071 },    // FemurL4ImpAngY
    { 0.894454, 1.014896, 0.405986 },    // FemurL4ImpAngZ
    { 0.583851, 0.754269, 0.000000 },    // FemurL4ImpRot
    { -0.958661, -1.038013, 0.324071 },  // FemurR1ImpAngX
    { -1.058661, -0.864808, 0.324071 },  // FemurR1ImpAngY
    { -0.958988, -0.922732, 0.405986 },  // FemurR1ImpAngZ
    { -0.607846, -0.720000, 0.000000 },  // FemurR1ImpRot
    { -1.074779, -0.420524, 0.324071 },  // FemurR2ImpAngX
    { -1.109508, -0.223563, 0.324071 },  // FemurR2ImpAngY
    { -1.035657, -0.312083, 0.405986 },  // FemurR2ImpAngZ
    { -0.636354, -0.241676, 0.000000 },  // FemurR2ImpRot
    { -1.104755, 0.285311, 0.324071 },   // FemurR3ImpAngX
    { -1.052991, 0.478496, 0.324071 },   // FemurR3ImpAngY
    { -1.023470, 0.367058, 0.405986 },   // FemurR3ImpAngZ
    { -0.631822, 0.262117, 0.000000 },   // FemurR3ImpRot
    { -1.002671, 0.975160, 0.324071 },   // FemurR4ImpAngX
    { -0.874113, 1.128369, 0.324071 },   // FemurR4ImpAngY
    { -0.894454, 1.014896, 0.405986 },   // FemurR4ImpAngZ
    { -0.583851, 0.754269, 0.000000 },   // FemurR4ImpRot
    { -0.100000, 0.722260, 0.032758 },   // HipImpAngX
    { 0.100000, 0.722260, 0.032758 },    // HipImpAngY
    { 0.000000, 0.696378, 0.129351 },    // HipImpAngZ
    { 0.000000, 0.600004, 0.000000 },    // HipImpRot
    { -0.100000, -0.709850, 0.109850 },  // NeckImpAngX
    { 0.100000, -0.709850, 0.109850 },   // NeckImpAngY
    { -0.000000, -0.639139, 0.180561 },  // NeckImpAngZ
    { 0.000000, -0.599997, -0.000003 },  // NeckImpRot
    { 1.700623, -1.466385, 0.578682 },   // PatellaL1ImpAngX
    { 1.800623, -1.293180, 0.578682 },   // PatellaL1ImpAngY
    { 1.765661, -1.388465, 0.677163 },   // PatellaL1ImpAngZ
    { 1.409475, -1.182821, 0.648141 },   // PatellaL1ImpRot
    { 1.918507, -0.569296, 0.578682 },   // PatellaL2ImpAngX
    { 1.953236, -0.372335, 0.578682 },   // PatellaL2ImpAngY
    { 1.952973, -0.473831, 0.677163 },   // PatellaL2ImpAngZ
    { 1.547933, -0.402412, 0.648141 },   // PatellaL2ImpRot
    { 1.932306, 0.507052, 0.578682 },    // PatellaL3ImpAngX
    { 1.880542, 0.700238, 0.578682 },    // PatellaL3ImpAngY
    { 1.923197, 0.608139, 0.677163 },    // PatellaL3ImpAngZ
    { 1.525923, 0.501690, 0.648141 },    // PatellaL3ImpRot
    { 1.658975, 1.525864, 0.578682 },    // PatellaL4ImpAngX
    { 1.530417, 1.679073, 0.578682 },    // PatellaL4ImpAngY
    { 1.607998, 1.613631, 0.677163 },    // PatellaL4ImpAngZ
    { 1.292933, 1.349260, 0.648141 },    // PatellaL4ImpRot
    { -1.700623, -1.466385, 0.578682 },  // PatellaR1ImpAngX
    { -1.800623, -1.293180, 0.578682 },  // PatellaR1ImpAngY
    { -1.765661, -1.388465, 0.677163 },  // PatellaR1ImpAngZ
    { -1.409475, -1.182821, 0.648141 },  // PatellaR1ImpRot
    { -1.918507, -0.569296, 0.578682 },  // PatellaR2ImpAngX
    { -1.953236, -0.372335, 0.578682 },  // PatellaR2ImpAngY
    { -1.952972, -0.473831, 0.677163 },  // PatellaR2ImpAngZ
    { -1.547933, -0.402412, 0.648141 },  // PatellaR2ImpRot
    { -1.932306, 0.507052, 0.578682 },   // PatellaR3ImpAngX
    { -1.880542, 0.700238, 0.578682 },   // PatellaR3ImpAngY
    { -1.923197, 0.608139, 0.677163 },   // PatellaR3ImpAngZ
    { -1.525923, 0.501690, 0.648141 },   // PatellaR3ImpRot
    { -1.658975, 1.525864, 0.578682 },   // PatellaR4ImpAngX
    { -1.530417, 1.679073, 0.578682 },   // PatellaR4ImpAngY
    { -1.607998, 1.613631, 0.677163 },   // PatellaR4ImpAngZ
    { -1.292933, 1.349260, 0.648141 },   // PatellaR4ImpRot
    { -0.100000, 0.000000, 0.000000 },   // SternumImpAngX
    { 0.100000, 0.000000, 0.000000 },    // SternumImpAngY
    { 0.000000, 0.000000, 0.100000 },    // SternumImpAngZ
    { 2.715063, -2.052072, -0.720684 },  // TarsusL1ImpAngX
    { 2.815063, -1.878867, -0.720684 },  // TarsusL1ImpAngY
    { 2.848714, -2.013766, -0.694802 },  // TarsusL1ImpAngZ
    { 2.707002, -1.931948, -0.470476 },  // TarsusL1ImpRot
    { 3.072088, -0.772704, -0.720687 },  // TarsusL2ImpAngX
    { 3.106817, -0.575742, -0.720687 },  // TarsusL2ImpAngY
    { 3.184577, -0.690996, -0.694806 },  // TarsusL2ImpAngZ
    { 3.023428, -0.662581, -0.470480 },  // TarsusL2ImpRot
    { 3.063769, 0.810227, -0.720687 },   // TarsusL3ImpAngX
    { 3.012005, 1.003412, -0.720687 },   // TarsusL3ImpAngY
    { 3.131188, 0.931820, -0.694806 },   // TarsusL3ImpAngZ
    { 2.973128, 0.889468, -0.470480 },   // TarsusL3ImpRot
    { 2.556302, 2.278811, -0.720687 },   // TarsusL4ImpAngX
    { 2.427744, 2.432019, -0.720687 },   // TarsusL4ImpAngY
    { 2.566017, 2.417504, -0.694806 },   // TarsusL4ImpAngZ
    { 2.440665, 2.312321, -0.470480 },   // TarsusL4ImpRot
    { -2.715065, -2.052073, -0.720687 }, // TarsusR1ImpAngX
    { -2.815065, -1.878868, -0.720687 }, // TarsusR1ImpAngY
    { -2.848716, -2.013767, -0.694806 }, // TarsusR1ImpAngZ
    { -2.707004, -1.931949, -0.470480 }, // TarsusR1ImpRot
    { -3.072088, -0.772704, -0.720687 }, // TarsusR2ImpAngX
    { -3.106817, -0.575742, -0.720687 }, // TarsusR2ImpAngY
    { -3.184577, -0.690996, -0.694806 }, // TarsusR2ImpAngZ
    { -3.023428, -0.662581, -0.470480 }, // TarsusR2ImpRot
    { -3.063769, 0.810227, -0.720687 },  // TarsusR3ImpAngX
    { -3.012005, 1.003412, -0.720687 },  // TarsusR3ImpAngY
    { -3.131188, 0.931820, -0.694806 },  // TarsusR3ImpAngZ
    { -2.973128, 0.889468, -0.470480 },  // TarsusR3ImpRot
    { -2.556302, 2.278811, -0.720687 },  // TarsusR4ImpAngX
    { -2.427744, 2.432019, -0.720687 },  // TarsusR4ImpAngY
    { -2.566017, 2.417504, -0.694806 },  // TarsusR4ImpAngZ
    { -2.440665, 2.312321, -0.470480 },  // TarsusR4ImpRot
    { 2.337326, -1.833986, 0.021827 },   // TibiaL1ImpAngX
    { 2.437326, -1.660781, 0.021827 },   // TibiaL1ImpAngY
    { 2.458266, -1.788341, 0.079185 },   // TibiaL1ImpAngZ
    { 2.091770, -1.576744, 0.509223 },   // TibiaL1ImpRot
    { 2.642539, -0.696963, 0.021827 },   // TibiaL2ImpAngX
    { 2.677268, -0.500001, 0.021827 },   // TibiaL2ImpAngY
    { 2.740574, -0.612706, 0.079185 },   // TibiaL2ImpAngZ
    { 2.323810, -0.539220, 0.509223 },   // TibiaL2ImpRot
    { 2.642456, 0.697337, 0.021827 },    // TibiaL3ImpAngX
    { 2.590692, 0.890522, 0.021827 },    // TibiaL3ImpAngY
    { 2.695698, 0.815130, 0.079185 },    // TibiaL3ImpAngZ
    { 2.286924, 0.705600, 0.509223 },    // TibiaL3ImpRot
    { 2.222172, 1.998443, 0.021827 },    // TibiaL4ImpAngX
    { 2.093614, 2.151651, 0.021827 },    // TibiaL4ImpAngY
    { 2.220643, 2.127701, 0.079185 },    // TibiaL4ImpAngZ
    { 1.896459, 1.855678, 0.509223 },    // TibiaL4ImpRot
    { -2.337326, -1.833986, 0.021827 },  // TibiaR1ImpAngX
    { -2.437325, -1.660781, 0.021827 },  // TibiaR1ImpAngY
    { -2.458266, -1.788341, 0.079185 },  // TibiaR1ImpAngZ
    { -2.091770, -1.576744, 0.509223 },  // TibiaR1ImpRot
    { -2.642538, -0.696963, 0.021827 },  // TibiaR2ImpAngX
    { -2.677268, -0.500001, 0.021827 },  // TibiaR2ImpAngY
    { -2.740574, -0.612706, 0.079185 },  // TibiaR2ImpAngZ
    { -2.323810, -0.539220, 0.509223 },  // TibiaR2ImpRot
    { -2.642456, 0.697337, 0.021827 },   // TibiaR3ImpAngX
    { -2.590692, 0.890522, 0.021827 },   // TibiaR3ImpAngY
    { -2.695698, 0.815130, 0.079185 },   // TibiaR3ImpAngZ
    { -2.286924, 0.705600, 0.509223 },   // TibiaR3ImpRot
    { -2.222172, 1.998443, 0.021827 },   // TibiaR4ImpAngX
    { -2.093614, 2.151651, 0.021827 },   // TibiaR4ImpAngY
    { -2.220643, 2.127701, 0.079185 },   // TibiaR4ImpAngZ
    { -1.896459, 1.855678, 0.509223 },   // TibiaR4ImpRot
    { 0.453923, -0.746603, -0.000000 },  // TrochanterL1ImpAngX
    { 0.553923, -0.573397, -0.000000 },  // TrochanterL1ImpAngY
    { 0.503923, -0.660000, 0.100000 },   // TrochanterL1ImpAngZ
    { 0.400000, -0.600000, -0.000000 },  // TrochanterL1ImpRot
    { 0.500812, -0.319319, 0.000000 },   // TrochanterL2ImpAngX
    { 0.535542, -0.122357, 0.000000 },   // TrochanterL2ImpAngY
    { 0.518177, -0.220838, 0.100000 },   // TrochanterL2ImpAngZ
    { 0.400000, -0.200000, 0.000000 },   // TrochanterL2ImpRot
    { 0.541793, 0.134466, 0.000000 },    // TrochanterL3ImpAngX
    { 0.490029, 0.327651, 0.000000 },    // TrochanterL3ImpAngY
    { 0.515911, 0.231058, 0.100000 },    // TrochanterL3ImpAngZ
    { 0.400000, 0.200000, 0.000000 },    // TrochanterL3ImpRot
    { 0.556204, 0.600530, -0.000000 },   // TrochanterL4ImpAngX
    { 0.427647, 0.753739, -0.000000 },   // TrochanterL4ImpAngY
    { 0.491925, 0.677134, 0.100000 },    // TrochanterL4ImpAngZ
    { 0.400000, 0.600000, -0.000000 },   // TrochanterL4ImpRot
    { -0.453923, -0.746603, 0.000000 },  // TrochanterR1ImpAngX
    { -0.553923, -0.573398, 0.000000 },  // TrochanterR1ImpAngY
    { -0.503923, -0.660000, 0.100000 },  // TrochanterR1ImpAngZ
    { -0.400000, -0.600000, 0.000000 },  // TrochanterR1ImpRot
    { -0.500812, -0.319319, 0.000000 },  // TrochanterR2ImpAngX
    { -0.535542, -0.122357, 0.000000 },  // TrochanterR2ImpAngY
    { -0.518177, -0.220838, 0.100000 },  // TrochanterR2ImpAngZ
    { -0.400000, -0.200000, 0.000000 },  // TrochanterR2ImpRot
    { -0.541793, 0.134466, 0.000000 },   // TrochanterR3ImpAngX
    { -0.490029, 0.327651, 0.000000 },   // TrochanterR3ImpAngY
    { -0.515911, 0.231058, 0.100000 },   // TrochanterR3ImpAngZ
    { -0.400000, 0.200000, 0.000000 },   // TrochanterR3ImpRot
    { -0.556204, 0.600530, 0.000000 },   // TrochanterR4ImpAngX
    { -0.427647, 0.753739, 0.000000 },   // TrochanterR4ImpAngY
    { -0.491925, 0.677134, 0.100000 },   // TrochanterR4ImpAngZ
    { -0.400000, 0.600000, 0.000000 },   // TrochanterR4ImpRot
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{
    // { 0.000000, 0.898278, 0.079924 },    // Abdomin
    // { 0.000000, -0.850000, 0.242041 },   // Eye
    { 0.607846, -0.720000, -0.000000 },  // FemurL1
    { 0.636354, -0.241676, 0.000000 },   // FemurL2
    { 0.631822, 0.262117, 0.000000 },    // FemurL3
    { 0.583851, 0.754269, 0.000000 },    // FemurL4
    { -0.607846, -0.720000, 0.000000 },  // FemurR1
    { -0.636354, -0.241676, 0.000000 },  // FemurR2
    { -0.631822, 0.262117, 0.000000 },   // FemurR3
    { -0.583851, 0.754269, 0.000000 },   // FemurR4
    // { 0.000000, 0.600004, 0.000000 },    // Hip
    // { 0.000000, -0.599997, -0.000003 },  // Neck
    { 1.409475, -1.182821, 0.648141 },   // PatellaL1
    { 1.547933, -0.402412, 0.648141 },   // PatellaL2
    { 1.525923, 0.501690, 0.648141 },    // PatellaL3
    { 1.292933, 1.349260, 0.648141 },    // PatellaL4
    { -1.409475, -1.182821, 0.648141 },  // PatellaR1
    { -1.547933, -0.402412, 0.648141 },  // PatellaR2
    { -1.525923, 0.501690, 0.648141 },   // PatellaR3
    { -1.292933, 1.349260, 0.648141 },   // PatellaR4
    // { 2.707002, -1.931948, -0.470476 },  // TarsusL1
    // { 3.023428, -0.662581, -0.470480 },  // TarsusL2
    // { 2.973128, 0.889468, -0.470480 },   // TarsusL3
    // { 2.440665, 2.312321, -0.470480 },   // TarsusL4
    // { -2.707004, -1.931949, -0.470480 }, // TarsusR1
    // { -3.023428, -0.662581, -0.470480 }, // TarsusR2
    // { -2.973128, 0.889468, -0.470480 },  // TarsusR3
    // { -2.440665, 2.312321, -0.470480 },  // TarsusR4
    { 2.091770, -1.576744, 0.509223 },   // TibiaL1
    { 2.323810, -0.539220, 0.509223 },   // TibiaL2
    { 2.286924, 0.705600, 0.509223 },    // TibiaL3
    { 1.896459, 1.855678, 0.509223 },    // TibiaL4
    { -2.091770, -1.576744, 0.509223 },  // TibiaR1
    { -2.323810, -0.539220, 0.509223 },  // TibiaR2
    { -2.286924, 0.705600, 0.509223 },   // TibiaR3
    { -1.896459, 1.855678, 0.509223 },   // TibiaR4
    { 0.400000, -0.600000, -0.000000 },  // TrochanterL1
    { 0.400000, -0.200000, 0.000000 },   // TrochanterL2
    { 0.400000, 0.200000, 0.000000 },    // TrochanterL3
    { 0.400000, 0.600000, -0.000000 },   // TrochanterL4
    { -0.400000, -0.600000, 0.000000 },  // TrochanterR1
    { -0.400000, -0.200000, 0.000000 },  // TrochanterR2
    { -0.400000, 0.200000, 0.000000 },   // TrochanterR3
    { -0.400000, 0.600000, 0.000000 }    // TrochanterR4
  };
  // clang-format on

  mSubstrate = new Substrate(inputs, hidden, outputs);

  mSubstrate->m_allow_input_hidden_links  = false;
  mSubstrate->m_allow_input_output_links  = false;
  mSubstrate->m_allow_hidden_hidden_links = false;
  mSubstrate->m_allow_hidden_output_links = false;
  mSubstrate->m_allow_output_hidden_links = false;
  mSubstrate->m_allow_output_output_links = false;
  mSubstrate->m_allow_looped_hidden_links = false;
  mSubstrate->m_allow_looped_output_links = false;

  mSubstrate->m_allow_input_hidden_links  = true;
  mSubstrate->m_allow_input_output_links  = false;
  mSubstrate->m_allow_hidden_output_links = true;
  mSubstrate->m_allow_hidden_hidden_links = false;

  mSubstrate->m_hidden_nodes_activation =
    NEAT::ActivationFunction::SIGNED_SIGMOID;
  mSubstrate->m_output_nodes_activation =
    NEAT::ActivationFunction::SIGNED_SIGMOID;

  mSubstrate->m_with_distance = true;

  mSubstrate->m_max_weight_and_bias = 8.0;
}

void SpiderSwarm::setDefaultPopulation() {

  // sanity checking
  if (mSubstrate == nullptr)
    throw std::runtime_error("Substrate is not set");
  if (mPopulation != nullptr)
    delete mPopulation;

  NEAT::Parameters params;

  // Basic parameters
  params.PopulationSize       = 50;
  params.DynamicCompatibility = true;
  params.MinSpecies           = 8;
  params.MaxSpecies           = 32;
  params.AllowClones          = true;

  // GA parameters
  params.YoungAgeTreshold                       = 15;
  params.YoungAgeFitnessBoost                   = 1.1;
  params.SpeciesMaxStagnation                   = 15;
  params.StagnationDelta                        = 0.0;
  params.OldAgeTreshold                         = 35;
  params.OldAgePenalty                          = 1.0;
  params.DetectCompetetiveCoevolutionStagnation = false;
  params.KillWorstSpeciesEach                   = 15;
  params.KillWorstAge                           = 10;
  params.SurvivalRate                           = 0.2;
  params.CrossoverRate                          = 0.75;
  params.OverallMutationRate                    = 0.2;
  params.InterspeciesCrossoverRate              = 0.001;
  params.MultipointCrossoverRate                = 0.4;
  params.RouletteWheelSelection                 = false;
  params.TournamentSize                         = 4;
  params.EliteFraction                          = 0.15;

  // Mutation parameters
  params.MutateAddNeuronProb       = 0.1;
  params.SplitRecurrent            = true;
  params.SplitLoopedRecurrent      = true;
  params.MutateAddLinkProb         = 0.2;
  params.MutateAddLinkFromBiasProb = 0.0;
  params.MutateRemLinkProb         = 0.0;
  params.MutateRemSimpleNeuronProb = 0.0;
  params.LinkTries                 = 32;
  params.RecurrentProb             = 0.50;
  params.RecurrentLoopProb         = 0.25;

  // Parameter mutation parameters
  params.MutateWeightsProb             = 0.8;
  params.MutateWeightsSevereProb       = 0.5;
  params.WeightMutationRate            = 0.25;
  params.WeightMutationMaxPower        = 0.5;
  params.WeightReplacementMaxPower     = 1.0;
  params.MaxWeight                     = 8.0;
  params.MutateActivationAProb         = 0.0;
  params.MutateActivationBProb         = 0.0;
  params.ActivationAMutationMaxPower   = 0.5;
  params.ActivationBMutationMaxPower   = 0.0;
  params.MinActivationA                = 1.1;
  params.MaxActivationA                = 6.9;
  params.MinActivationB                = 0.0;
  params.MaxActivationB                = 0.0;
  params.TimeConstantMutationMaxPower  = 0.1;
  params.MutateNeuronTimeConstantsProb = 0.0;
  params.MutateNeuronBiasesProb        = 0.1;
  params.MinNeuronTimeConstant         = 0.04;
  params.MaxNeuronTimeConstant         = 0.24;
  params.MinNeuronBias                 = -8.0;
  params.MaxNeuronBias                 = 8.0;

  // Activation function parameters
  params.MutateNeuronActivationTypeProb          = 0.0;
  params.ActivationFunction_SignedSigmoid_Prob   = 1.0;
  params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
  params.ActivationFunction_Tanh_Prob            = 1.0;
  params.ActivationFunction_TanhCubic_Prob       = 0.0;
  params.ActivationFunction_SignedStep_Prob      = 1.0;
  params.ActivationFunction_UnsignedStep_Prob    = 0.0;
  params.ActivationFunction_SignedGauss_Prob     = 1.0;
  params.ActivationFunction_UnsignedGauss_Prob   = 0.0;
  params.ActivationFunction_Abs_Prob             = 1.0;
  params.ActivationFunction_SignedSine_Prob      = 1.0;
  params.ActivationFunction_UnsignedSine_Prob    = 0.0;
  params.ActivationFunction_Linear_Prob          = 0.0;
  params.ActivationFunction_Relu_Prob            = 0.0;
  params.ActivationFunction_Softplus_Prob        = 0.0;


  params.BiasMutationMaxPower = 0.5;

  // Genome properties parameters
  // params.DontUseBiasNeuron                       = false;
  // params.AllowLoops                              = true;

  // Speciation parameters
  params.DisjointCoeff                         = 1.0;
  params.ExcessCoeff                           = 1.0;
  params.WeightDiffCoeff                       = 1.0;
  params.ActivationADiffCoeff                  = 0.0;
  params.ActivationBDiffCoeff                  = 0.0;
  params.TimeConstantDiffCoeff                 = 0.0;
  params.BiasDiffCoeff                         = 0.0;
  params.ActivationFunctionDiffCoeff           = 0.0;
  params.CompatTreshold                        = 2.0;
  params.MinCompatTreshold                     = 0.2;
  params.CompatTresholdModifier                = 0.3;
  params.CompatTreshChangeInterval_Generations = 1;
  params.CompatTreshChangeInterval_Evaluations = 10;

  // ES-HyperNEAT parameters
  params.DivisionThreshold = 0.03;
  params.VarianceThreshold = 0.03;
  params.BandThreshold     = 0.3;
  params.InitialDepth      = 3;
  params.MaxDepth          = 4;
  params.IterationLevel    = 2;
  params.CPPN_Bias         = 1.0;
  params.Width             = 3.2;
  params.Height            = 3.2;
  params.Qtree_X           = 0.0;
  params.Qtree_Y           = 0.0;
  params.Leo               = false;
  params.LeoThreshold      = 0.3;
  params.LeoSeed           = false;
  params.GeometrySeed      = false;

  NEAT::Genome genome(0,
                      mSubstrate->GetMinCPPNInputs(),
                      0,
                      mSubstrate->GetMinCPPNOutputs(),
                      false,
                      NEAT::ActivationFunction::TANH,
                      NEAT::ActivationFunction::TANH,
                      0,
                      params);

  mPopulation = new NEAT::Population(genome, params, true, 1.0, time(0));
}
