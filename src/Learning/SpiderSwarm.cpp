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
    , mIterationDuration(11)
    , mBestPossibleFitness(-99999)
    , mBestPossibleFitnessGeneration(-99999)
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

  mBuildingHyperNeatWorker = [](std::vector<Phenotype>::iterator begin,
                                  std::vector<Phenotype>::iterator end,
                                  NEAT::Population&                pop,
                                  Substrate&                       sub) {
    for (auto it = begin; it != end; ++it) {
      pop.m_Species[it->speciesIndex]
        .m_Individuals[it->individualIndex]
        .BuildHyperNEATPhenotype(*it->network, sub);
    }
  };
#endif

  NEAT::RNG rng;
  rng.TimeSeed();

  setDefaultSubstrate();
  setDefaultPopulation();

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
  mStats               = Statistics();

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
 *   Returns the set iteration duration
 *
 * @return
 */
float SpiderSwarm::iterationDuration() {
  return mIterationDuration;
}

/**
 * @brief
 *   Sets the simulation duration of the phenotypes.
 *   The value represents seconds
 *
 * @param x
 */
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

  float  best      = -99999.f;
  size_t bestIndex = 0;

  ++mGeneration;

  size_t index = 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    float  bestOfSpecies = -99999.f;
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
        mBestPossibleFitnessGeneration = mGeneration;

        // also store it to file for future reference
        save("current-g" + std::to_string(mGeneration));
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
               p.speciesId,
               p.individualIndex,
               p.finalizedFitness,
               i == mBestIndex ? " (best)" : "",
               p.hasBeenKilled() ? " (killed)" : "");

    size_t j = 0;
    int maxLength = 0;

    // Find the longest name so alignment can be done
    for (auto& f : Phenotype::FITNESS_HANDLERS)
      maxLength = mmm::max(f.name().size(), maxLength);

    // Print out aligned fitness names and their values
    for (auto& f : Phenotype::FITNESS_HANDLERS) {
      std::string name =
        f.name() + std::string(" ", mmm::max(maxLength - f.name().size(), 0));

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

      mPhenotypes[index].reset(species.ID(), i, j, g.GetID(), mNumInputs);
      mPhenotypes[index].spider->disableUpdatingFromPhysics();

      // If we are using single-threaded mode, create the neural
      // networks, otherwise wait until later
#ifndef BT_NO_PROFILE
      auto& individual = species.m_Individuals[j];
      individual.BuildHyperNEATPhenotype(*mPhenotypes[index].network,
                                           *mSubstrate);
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
    *it = std::thread(mBuildingHyperNeatWorker,
                      workIter,
                      workIter + grainSize,
                      std::ref(*mPopulation),
                      std::ref(*mSubstrate));
    workIter += grainSize;
  }

  threads.back() = std::thread(mBuildingHyperNeatWorker,
                               workIter,
                               std::end(mPhenotypes),
                               std::ref(*mPopulation),
                               std::ref(*mSubstrate));

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
    {  0.0, -0.6, -0.4 }, // Phase -cos
    {  0.0, -0.2, -0.4 }, // Phase -sin
    {  0.0,  0.2, -0.4 }, // Phase  cos
    {  0.0,  0.6, -0.4 }, // Phase  sin

    {  0.0, -0.4, -0.4 }, // Rotation sternum z
    {  0.0,  0.0, -0.4 }, // Rotation sternum y
    {  0.0,  0.4, -0.4 }, // Rotation sternum x

    {  1.2, -0.6, -0.4 }, // TipL1
    {  1.2, -0.2, -0.4 }, // TipL2
    {  1.2,  0.2, -0.4 }, // TipL3
    {  1.2,  0.6, -0.4 }, // TipL4
    { -1.2, -0.6, -0.4 }, // TipR1
    { -1.2, -0.2, -0.4 }, // TipR2
    { -1.2,  0.2, -0.4 }, // TipR3
    { -1.2,  0.6, -0.4 }, // TipR4

    {  1.0, -0.4, -0.4 }, // Tarsus L1 L2 Diff
    {  1.0,  0.0, -0.4 }, // Tarsus L2 L3 Diff
    {  1.0,  0.4, -0.4 }, // Tarsus L3 L4 Diff
    { -1.0, -0.4, -0.4 }, // Tarsus R1 R2 Diff
    { -1.0,  0.0, -0.4 }, // Tarsus R2 R3 Diff
    { -1.0,  0.4, -0.4 }, // Tarsus R3 R4 Diff

    {  0.4, -0.6, -0.4 }, // FemurL1
    {  0.4, -0.2, -0.4 }, // FemurL2
    {  0.4,  0.2, -0.4 }, // FemurL3
    {  0.4,  0.6, -0.4 }, // FemurL4
    { -0.4, -0.6, -0.4 }, // FemurR1
    { -0.4, -0.2, -0.4 }, // FemurR2
    { -0.4,  0.2, -0.4 }, // FemurR3
    { -0.4,  0.6, -0.4 }, // FemurR4
    {  0.6, -0.6, -0.4 }, // PatellaL1
    {  0.6, -0.2, -0.4 }, // PatellaL2
    {  0.6,  0.2, -0.4 }, // PatellaL3
    {  0.6,  0.6, -0.4 }, // PatellaL4
    { -0.6, -0.6, -0.4 }, // PatellaR1
    { -0.6, -0.2, -0.4 }, // PatellaR2
    { -0.6,  0.2, -0.4 }, // PatellaR3
    { -0.6,  0.6, -0.4 }, // PatellaR4
    {  1.0, -0.6, -0.4 }, // TarsusL1
    {  1.0, -0.2, -0.4 }, // TarsusL2
    {  1.0,  0.2, -0.4 }, // TarsusL3
    {  1.0,  0.6, -0.4 }, // TarsusL4
    { -1.0, -0.6, -0.4 }, // TarsusR1
    { -1.0, -0.2, -0.4 }, // TarsusR2
    { -1.0,  0.2, -0.4 }, // TarsusR3
    { -1.0,  0.6, -0.4 }, // TarsusR4
    {  0.8, -0.6, -0.4 }, // TibiaL1
    {  0.8, -0.2, -0.4 }, // TibiaL2
    {  0.8,  0.2, -0.4 }, // TibiaL3
    {  0.8,  0.6, -0.4 }, // TibiaL4
    { -0.8, -0.6, -0.4 }, // TibiaR1
    { -0.8, -0.2, -0.4 }, // TibiaR2
    { -0.8,  0.2, -0.4 }, // TibiaR3
    { -0.8,  0.6, -0.4 }, // TibiaR4
    {  0.2, -0.6, -0.4 }, // TrochanterL1
    {  0.2, -0.2, -0.4 }, // TrochanterL2
    {  0.2,  0.2, -0.4 }, // TrochanterL3
    {  0.2,  0.6, -0.4 }, // TrochanterL4
    { -0.2, -0.6, -0.4 }, // TrochanterR1
    { -0.2, -0.2, -0.4 }, // TrochanterR2
    { -0.2,  0.2, -0.4 }, // TrochanterR3
    { -0.2,  0.6, -0.4 }  // TrochanterR4
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{
    {  0.4, -0.6,  0.4 }, // FemurL1
    {  0.4, -0.2,  0.4 }, // FemurL2
    {  0.4,  0.2,  0.4 }, // FemurL3
    {  0.4,  0.6,  0.4 }, // FemurL4
    { -0.4, -0.6,  0.4 }, // FemurR1
    { -0.4, -0.2,  0.4 }, // FemurR2
    { -0.4,  0.2,  0.4 }, // FemurR3
    { -0.4,  0.6,  0.4 }, // FemurR4
    {  0.6, -0.6,  0.4 }, // PatellaL1
    {  0.6, -0.2,  0.4 }, // PatellaL2
    {  0.6,  0.2,  0.4 }, // PatellaL3
    {  0.6,  0.6,  0.4 }, // PatellaL4
    { -0.6, -0.6,  0.4 }, // PatellaR1
    { -0.6, -0.2,  0.4 }, // PatellaR2
    { -0.6,  0.2,  0.4 }, // PatellaR3
    { -0.6,  0.6,  0.4 }, // PatellaR4
    {  1.0, -0.6,  0.4 }, // TarsusL1
    {  1.0, -0.2,  0.4 }, // TarsusL2
    {  1.0,  0.2,  0.4 }, // TarsusL3
    {  1.0,  0.6,  0.4 }, // TarsusL4
    { -1.0, -0.6,  0.4 }, // TarsusR1
    { -1.0, -0.2,  0.4 }, // TarsusR2
    { -1.0,  0.2,  0.4 }, // TarsusR3
    { -1.0,  0.6,  0.4 }, // TarsusR4
    {  0.8, -0.6,  0.4 }, // TibiaL1
    {  0.8, -0.2,  0.4 }, // TibiaL2
    {  0.8,  0.2,  0.4 }, // TibiaL3
    {  0.8,  0.6,  0.4 }, // TibiaL4
    { -0.8, -0.6,  0.4 }, // TibiaR1
    { -0.8, -0.2,  0.4 }, // TibiaR2
    { -0.8,  0.2,  0.4 }, // TibiaR3
    { -0.8,  0.6,  0.4 }, // TibiaR4
    {  0.2, -0.6,  0.4 }, // TrochanterL1
    {  0.2, -0.2,  0.4 }, // TrochanterL2
    {  0.2,  0.2,  0.4 }, // TrochanterL3
    {  0.2,  0.6,  0.4 }, // TrochanterL4
    { -0.2, -0.6,  0.4 }, // TrochanterR1
    { -0.2, -0.2,  0.4 }, // TrochanterR2
    { -0.2,  0.2,  0.4 }, // TrochanterR3
    { -0.2,  0.6,  0.4 }  // TrochanterR4
  };
  // clang-format on


  // clone the input neuron positions to hidden, but at different height
  for (auto& x : inputs)
    hidden.push_back(std::vector<double>{x[0], x[1],  0.0});


  mSubstrate = new Substrate(inputs, hidden, outputs);

  // These variables are only used in HyperNEAT variations and not
  // in ESHyperNEAT.
  mSubstrate->m_allow_input_hidden_links  = true;
  mSubstrate->m_allow_input_output_links  = true;
  mSubstrate->m_allow_hidden_output_links = true;
  mSubstrate->m_allow_output_hidden_links = true;
  mSubstrate->m_allow_output_output_links = true;
  mSubstrate->m_allow_looped_hidden_links = true;
  mSubstrate->m_allow_looped_output_links = true;

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
  mSubstrate->m_max_weight_and_bias = 8.0;
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
  params.PopulationSize = 64;

  // If true, this enables dynamic compatibility thresholding
  // It will keep the number of species between MinSpecies and MaxSpecies
  params.DynamicCompatibility = true;

  // Minimum number of species
  params.MinSpecies = 5;

  // Maximum number of species
  params.MaxSpecies = 10;

  // Don't wipe the innovation database each generation?
  params.InnovationsForever = false;

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
  params.IterationLevel = 2;

  // The Bias value for the CPPN queries.
  params.CPPN_Bias = 1.0;

  // Quadtree Dimensions
  // The range of the tree. Typically set to 2,
  params.Width = 1.2;
  params.Height = 0.6;

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
