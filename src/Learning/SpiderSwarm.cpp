#include "SpiderSwarm.hpp"

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"
#include "Substrate.hpp"
#include "DrawablePhenotype.hpp"

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
  {-4, 0, -4}, {4, 0, -4}, {4, 0, 4},  {-4, 0, 4},

  {-12, 0, 4}, {-12, 0, -4}, {-12, 0, -12},  {-4, 0, -12},

  {4, 0, -12}, {12, 0, -12}, {12, 0, -4},  {12, 0, 4},

  {12, 0, 12}, {4, 0, 12}, {-4, 0, 12},  {-12, 0, 12},
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
    , mDrawingMethod(SpiderSwarm::DrawingMethod::SpeciesLeaders)
    , mBestIndex(0)
    , mParameters(nullptr)
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

  setDefaultParameters();
  setDefaultSubstrate();
  setDefaultPopulation();

  mPhenotypes.reserve(mParameters->PopulationSize);

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
  delete mParameters;
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
 *   This is the main update function that controls what to do next.
 *   Most of the time this will update the spiders by activating the networks,
 *   using the networks output before running physics.
 *
 *   When a batch is complete, the next one is chosen. Once all batches are
 *   complete, the next epoch is started, resetting all the spiders to their
 *   original position.
 */
void SpiderSwarm::update(float deltaTime) {
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
 *   Draws a set number of spiders from the current batch being processed. This
 *   is set by the DrawLimit.
 *
 * @param prog
 * @param bindTexture
 */
void SpiderSwarm::draw(std::shared_ptr<Program>& prog, bool bindTexture) {
  size_t numPhenotypes = mPhenotypes.size();
  size_t gridIndex = 0;
  size_t gridSize  = grid.size();

  switch(mDrawingMethod) {
    case DrawingMethod::SpeciesLeaders: {
      for(auto& a : mSpeciesLeaders) {
        if (a < numPhenotypes && gridIndex < gridSize) {
          mPhenotypes[a].spider->enableUpdatingFromPhysics();
          mPhenotypes[a].spider->draw(prog, grid[gridIndex], bindTexture);

          if (bindTexture)
            mPhenotypes[a].drawablePhenotype->draw3D(grid[gridIndex] + mmm::vec3(0, 5, 0));
        }

        gridIndex++;
      }

      break;
    }
    case DrawingMethod::BestFitness:
      if (mBestIndex < numPhenotypes) {
        mPhenotypes[mBestIndex].spider->enableUpdatingFromPhysics();
        mPhenotypes[mBestIndex].spider->draw(prog, bindTexture);
        if (bindTexture)
         mPhenotypes[mBestIndex].drawablePhenotype->draw3D(grid[gridIndex] + mmm::vec3(0, 5, 0));
      }
      break;
    case DrawingMethod::DrawAll:
      for(auto& p : mPhenotypes) {
        if (gridIndex < gridSize) {
          p.spider->enableUpdatingFromPhysics();
          p.spider->draw(prog, grid[gridIndex], bindTexture);
          if (bindTexture) {
            p.drawablePhenotype->draw3D(grid[gridIndex] + mmm::vec3(0, 5, 0));
          }
        }
        gridIndex++;
      }

      break;
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
  mParameters->Save(paramFilename.c_str());
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
  mParameters->Load(paramFilename.c_str());
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
    size_t leaderIndex = 0;

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
  mLog->info("The index being drawn is now: {}", mBestIndex);

  mLog->info("Breakdown of fitness:");
  mLog->info("  0: {} %", mPhenotypes[mBestIndex].fitness[0] * 100);
  mLog->info("  1: {} %", mPhenotypes[mBestIndex].fitness[1] * 100);
  mLog->info("  2: {} %", mPhenotypes[mBestIndex].fitness[2] * 100);
  mLog->info("  3: {} %", mPhenotypes[mBestIndex].fitness[3] * 100);
  mLog->info("  4: {} %", mPhenotypes[mBestIndex].fitness[4] * 100);
  mLog->info("  5: {} %", mPhenotypes[mBestIndex].fitness[5] * 100);
  mLog->info("  6: {} %", mPhenotypes[mBestIndex].fitness[6] * 100);
  mLog->info("  7: {} %", mPhenotypes[mBestIndex].fitness[7] * 100);
  mLog->info("  8: {} %", mPhenotypes[mBestIndex].fitness[8] * 100);

  mPopulation->Epoch();
  recreatePhenotypes();

  // mLog->info("saving population...");
  // save("testswarm");
}

void SpiderSwarm::recreatePhenotypes() {
  mLog->debug("Recreating {} phenotypes...", mParameters->PopulationSize);
  mLog->debug("We have {} species", mPopulation->m_Species.size());

  size_t index = 0;
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

#ifndef BT_NO_PROFILE
      auto& individual = species.m_Individuals[j];
      individual.BuildESHyperNEATPhenotype(*mPhenotypes[index].network,
                                           *mSubstrate,
                                           *mParameters);
#endif
      ++index;
    }
  }

  while (index < mPhenotypes.size()) {
    mPhenotypes.back().remove();
    mPhenotypes.pop_back();
    mLog->debug("Removing spider due to decrease in population");
  }

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
                      std::ref(*mParameters));
    workIter += grainSize;
  }

  threads.back() = std::thread(mBuildingESHyperNeatWorker,
                               workIter,
                               std::end(mPhenotypes),
                               std::ref(*mPopulation),
                               std::ref(*mSubstrate),
                               std::ref(*mParameters));

  for (auto&& i : threads)
    i.join();

#endif

  for(auto& i : mPhenotypes) {
    i.drawablePhenotype->recreate(*i.network, mmm::vec3(1.0, 1.0, 1.0));
  }

  mLog->debug("Created {} spiders", mPhenotypes.size());
}

void SpiderSwarm::setDefaultParameters() {
  if (mParameters != nullptr)
    delete mParameters;

  mParameters = new NEAT::Parameters();

  // Basic parameters
  mParameters->PopulationSize                          = 50;
  mParameters->DynamicCompatibility                    = true;
  mParameters->MinSpecies                              = 5;
  mParameters->MaxSpecies                              = 10;
  mParameters->AllowClones                             = true;

  // GA parameters
  mParameters->YoungAgeTreshold                        = 15;
  mParameters->YoungAgeFitnessBoost                    = 1.1;
  mParameters->SpeciesMaxStagnation                    = 15;
  mParameters->StagnationDelta                         = 0.0;
  mParameters->OldAgeTreshold                          = 35;
  mParameters->OldAgePenalty                           = 1.0;
  mParameters->DetectCompetetiveCoevolutionStagnation  = false;
  mParameters->KillWorstSpeciesEach                    = 15;
  mParameters->KillWorstAge                            = 10;
  mParameters->SurvivalRate                            = 0.2;
  mParameters->CrossoverRate                           = 0.75;
  mParameters->OverallMutationRate                     = 0.2;
  mParameters->InterspeciesCrossoverRate               = 0.001;
  mParameters->MultipointCrossoverRate                 = 0.4;
  mParameters->RouletteWheelSelection                  = false;
  mParameters->TournamentSize                          = 4;
  mParameters->EliteFraction                           = 0.15;

  // Mutation parameters
  mParameters->MutateAddNeuronProb                     = 0.1;
  mParameters->SplitRecurrent                          = true;
  mParameters->SplitLoopedRecurrent                    = true;
  mParameters->MutateAddLinkProb                       = 0.2;
  mParameters->MutateAddLinkFromBiasProb               = 0.0;
  mParameters->MutateRemLinkProb                       = 0.0;
  mParameters->MutateRemSimpleNeuronProb               = 0.0;
  mParameters->LinkTries                               = 32;
  mParameters->RecurrentProb                           = 0.50;
  mParameters->RecurrentLoopProb                       = 0.25;

  // Parameter mutation parameters
  mParameters->MutateWeightsProb                       = 0.8;
  mParameters->MutateWeightsSevereProb                 = 0.5;
  mParameters->WeightMutationRate                      = 0.25;
  mParameters->WeightMutationMaxPower                  = 0.5;
  mParameters->WeightReplacementMaxPower               = 1.0;
  mParameters->MaxWeight                               = 8.0;
  mParameters->MutateActivationAProb                   = 0.0;
  mParameters->MutateActivationBProb                   = 0.0;
  mParameters->ActivationAMutationMaxPower             = 0.5;
  mParameters->ActivationBMutationMaxPower             = 0.0;
  mParameters->MinActivationA                          = 1.1;
  mParameters->MaxActivationA                          = 6.9;
  mParameters->MinActivationB                          = 0.0;
  mParameters->MaxActivationB                          = 0.0;
  mParameters->TimeConstantMutationMaxPower            = 0.1;
  mParameters->MutateNeuronTimeConstantsProb           = 0.0;
  mParameters->MutateNeuronBiasesProb                  = 0.1;
  mParameters->MinNeuronTimeConstant                   = 0.04;
  mParameters->MaxNeuronTimeConstant                   = 0.24;
  mParameters->MinNeuronBias                           = -8.0;
  mParameters->MaxNeuronBias                           = 8.0;

  // Activation function parameters
  mParameters->MutateNeuronActivationTypeProb          = 0.0;
  mParameters->ActivationFunction_SignedSigmoid_Prob   = 0.0;
  mParameters->ActivationFunction_UnsignedSigmoid_Prob = 0.0;
  mParameters->ActivationFunction_Tanh_Prob            = 1.0;
  mParameters->ActivationFunction_TanhCubic_Prob       = 0.0;
  mParameters->ActivationFunction_SignedStep_Prob      = 0.0;
  mParameters->ActivationFunction_UnsignedStep_Prob    = 0.0;
  mParameters->ActivationFunction_SignedGauss_Prob     = 0.0;
  mParameters->ActivationFunction_UnsignedGauss_Prob   = 0.0;
  mParameters->ActivationFunction_Abs_Prob             = 0.0;
  mParameters->ActivationFunction_SignedSine_Prob      = 0.0;
  mParameters->ActivationFunction_UnsignedSine_Prob    = 0.0;
  mParameters->ActivationFunction_Linear_Prob          = 0.0;
  mParameters->ActivationFunction_Relu_Prob            = 0.0;
  mParameters->ActivationFunction_Softplus_Prob        = 0.0;


  mParameters->BiasMutationMaxPower = 0.5;

  // Genome properties parameters
  // mParameters->DontUseBiasNeuron                       = false;
  // mParameters->AllowLoops                              = true;

  // Speciation parameters
  mParameters->DisjointCoeff                           = 1.0;
  mParameters->ExcessCoeff                             = 1.0;
  mParameters->WeightDiffCoeff                         = 1.0;
  mParameters->ActivationADiffCoeff                    = 0.0;
  mParameters->ActivationBDiffCoeff                    = 0.0;
  mParameters->TimeConstantDiffCoeff                   = 0.0;
  mParameters->BiasDiffCoeff                           = 0.0;
  mParameters->ActivationFunctionDiffCoeff             = 0.0;
  mParameters->CompatTreshold                          = 2.0;
  mParameters->MinCompatTreshold                       = 0.2;
  mParameters->CompatTresholdModifier                  = 0.3;
  mParameters->CompatTreshChangeInterval_Generations   = 1;
  mParameters->CompatTreshChangeInterval_Evaluations   = 10;

  // ES-HyperNEAT parameters
  mParameters->DivisionThreshold                       = 0.03;
  mParameters->VarianceThreshold                       = 0.03;
  mParameters->BandThreshold                           = 0.3;
  mParameters->InitialDepth                            = 3;
  mParameters->MaxDepth                                = 4;
  mParameters->IterationLevel                          = 1;
  mParameters->CPPN_Bias                               = 1.0;
  mParameters->Width                                   = 2.0;
  mParameters->Height                                  = 2.0;
  mParameters->Qtree_X                                 = 0.0;
  mParameters->Qtree_Y                                 = 0.0;
  mParameters->Leo                                     = false;
  mParameters->LeoThreshold                            = 0.3;
  mParameters->LeoSeed                                 = false;
  mParameters->GeometrySeed                            = false;
}
void SpiderSwarm::setDefaultSubstrate() {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  // clang-format off
  std::vector<std::vector<double>> inputs{
    { 0.000000, 0.000000, 0.000000 },    // Phase,
    { -0.100000, 1.565926, 0.258819 },   // AbdominImpAngX,
    { 0.100000, 1.565926, 0.258819 },    // AbdominImpAngY,
    { 0.000000, 1.540044, 0.355412 },    // AbdominImpAngZ,
    { -0.100000, -0.850000, 0.446689 },  // EyeImpAngX,
    { 0.100000, -0.850000, 0.446689 },   // EyeImpAngY,
    { 0.000000, -0.850000, 0.546689 },   // EyeImpAngZ,
    { 0.958661, -1.038013, 0.324071 },   // FemurL1ImpAngX,
    { 1.058661, -0.864808, 0.324071 },   // FemurL1ImpAngY,
    { 0.958988, -0.922732, 0.405986 },   // FemurL1ImpAngZ,
    { 1.074779, -0.420524, 0.324071 },   // FemurL2ImpAngX,
    { 1.109508, -0.223563, 0.324071 },   // FemurL2ImpAngY,
    { 1.035657, -0.312084, 0.405986 },   // FemurL2ImpAngZ,
    { 1.104755, 0.285311, 0.324071 },    // FemurL3ImpAngX,
    { 1.052991, 0.478496, 0.324071 },    // FemurL3ImpAngY,
    { 1.023470, 0.367058, 0.405986 },    // FemurL3ImpAngZ,
    { 1.002671, 0.975160, 0.324071 },    // FemurL4ImpAngX,
    { 0.874113, 1.128369, 0.324071 },    // FemurL4ImpAngY,
    { 0.894454, 1.014896, 0.405986 },    // FemurL4ImpAngZ,
    { -0.958661, -1.038013, 0.324071 },  // FemurR1ImpAngX,
    { -1.058661, -0.864808, 0.324071 },  // FemurR1ImpAngY,
    { -0.958988, -0.922732, 0.405986 },  // FemurR1ImpAngZ,
    { -1.074779, -0.420524, 0.324071 },  // FemurR2ImpAngX,
    { -1.109508, -0.223563, 0.324071 },  // FemurR2ImpAngY,
    { -1.035657, -0.312083, 0.405986 },  // FemurR2ImpAngZ,
    { -1.104755, 0.285311, 0.324071 },   // FemurR3ImpAngX,
    { -1.052991, 0.478496, 0.324071 },   // FemurR3ImpAngY,
    { -1.023470, 0.367058, 0.405986 },   // FemurR3ImpAngZ,
    { -1.002671, 0.975160, 0.324071 },   // FemurR4ImpAngX,
    { -0.874113, 1.128369, 0.324071 },   // FemurR4ImpAngY,
    { -0.894454, 1.014896, 0.405986 },   // FemurR4ImpAngZ,
    { -0.100000, 0.722260, 0.032758 },   // HipImpAngX,
    { 0.100000, 0.722260, 0.032758 },    // HipImpAngY,
    { 0.000000, 0.696378, 0.129351 },    // HipImpAngZ,
    { -0.100000, -0.709850, 0.109850 },  // NeckImpAngX,
    { 0.100000, -0.709850, 0.109850 },   // NeckImpAngY,
    { -0.000000, -0.639139, 0.180561 },  // NeckImpAngZ,
    { 1.700623, -1.466385, 0.578682 },   // PatellaL1ImpAngX,
    { 1.800623, -1.293180, 0.578682 },   // PatellaL1ImpAngY,
    { 1.765661, -1.388465, 0.677163 },   // PatellaL1ImpAngZ,
    { 1.918507, -0.569296, 0.578682 },   // PatellaL2ImpAngX,
    { 1.953236, -0.372335, 0.578682 },   // PatellaL2ImpAngY,
    { 1.952973, -0.473831, 0.677163 },   // PatellaL2ImpAngZ,
    { 1.932306, 0.507052, 0.578682 },    // PatellaL3ImpAngX,
    { 1.880542, 0.700238, 0.578682 },    // PatellaL3ImpAngY,
    { 1.923197, 0.608139, 0.677163 },    // PatellaL3ImpAngZ,
    { 1.658975, 1.525864, 0.578682 },    // PatellaL4ImpAngX,
    { 1.530417, 1.679073, 0.578682 },    // PatellaL4ImpAngY,
    { 1.607998, 1.613631, 0.677163 },    // PatellaL4ImpAngZ,
    { -1.700623, -1.466385, 0.578682 },  // PatellaR1ImpAngX,
    { -1.800623, -1.293180, 0.578682 },  // PatellaR1ImpAngY,
    { -1.765661, -1.388465, 0.677163 },  // PatellaR1ImpAngZ,
    { -1.918507, -0.569296, 0.578682 },  // PatellaR2ImpAngX,
    { -1.953236, -0.372335, 0.578682 },  // PatellaR2ImpAngY,
    { -1.952972, -0.473831, 0.677163 },  // PatellaR2ImpAngZ,
    { -1.932306, 0.507052, 0.578682 },   // PatellaR3ImpAngX,
    { -1.880542, 0.700238, 0.578682 },   // PatellaR3ImpAngY,
    { -1.923197, 0.608139, 0.677163 },   // PatellaR3ImpAngZ,
    { -1.658975, 1.525864, 0.578682 },   // PatellaR4ImpAngX,
    { -1.530417, 1.679073, 0.578682 },   // PatellaR4ImpAngY,
    { -1.607998, 1.613631, 0.677163 },   // PatellaR4ImpAngZ,
    { -0.100000, 0.000000, 0.000000 },   // SternumImpAngX,
    { 0.100000, 0.000000, 0.000000 },    // SternumImpAngY,
    { 0.000000, 0.000000, 0.100000 },    // SternumImpAngZ,
    { 2.715063, -2.052072, -0.720684 },  // TarsusL1ImpAngX,
    { 2.815063, -1.878867, -0.720684 },  // TarsusL1ImpAngY,
    { 2.848714, -2.013766, -0.694802 },  // TarsusL1ImpAngZ,
    { 3.072088, -0.772704, -0.720687 },  // TarsusL2ImpAngX,
    { 3.106817, -0.575742, -0.720687 },  // TarsusL2ImpAngY,
    { 3.184577, -0.690996, -0.694806 },  // TarsusL2ImpAngZ,
    { 3.063769, 0.810227, -0.720687 },   // TarsusL3ImpAngX,
    { 3.012005, 1.003412, -0.720687 },   // TarsusL3ImpAngY,
    { 3.131188, 0.931820, -0.694806 },   // TarsusL3ImpAngZ,
    { 2.556302, 2.278811, -0.720687 },   // TarsusL4ImpAngX,
    { 2.427744, 2.432019, -0.720687 },   // TarsusL4ImpAngY,
    { 2.566017, 2.417504, -0.694806 },   // TarsusL4ImpAngZ,
    { -2.715065, -2.052073, -0.720687 }, // TarsusR1ImpAngX,
    { -2.815065, -1.878868, -0.720687 }, // TarsusR1ImpAngY,
    { -2.848716, -2.013767, -0.694806 }, // TarsusR1ImpAngZ,
    { -3.072088, -0.772704, -0.720687 }, // TarsusR2ImpAngX,
    { -3.106817, -0.575742, -0.720687 }, // TarsusR2ImpAngY,
    { -3.184577, -0.690996, -0.694806 }, // TarsusR2ImpAngZ,
    { -3.063769, 0.810227, -0.720687 },  // TarsusR3ImpAngX,
    { -3.012005, 1.003412, -0.720687 },  // TarsusR3ImpAngY,
    { -3.131188, 0.931820, -0.694806 },  // TarsusR3ImpAngZ,
    { -2.556302, 2.278811, -0.720687 },  // TarsusR4ImpAngX,
    { -2.427744, 2.432019, -0.720687 },  // TarsusR4ImpAngY,
    { -2.566017, 2.417504, -0.694806 },  // TarsusR4ImpAngZ,
    { 2.337326, -1.833986, 0.021827 },   // TibiaL1ImpAngX,
    { 2.437326, -1.660781, 0.021827 },   // TibiaL1ImpAngY,
    { 2.458266, -1.788341, 0.079185 },   // TibiaL1ImpAngZ,
    { 2.642539, -0.696963, 0.021827 },   // TibiaL2ImpAngX,
    { 2.677268, -0.500001, 0.021827 },   // TibiaL2ImpAngY,
    { 2.740574, -0.612706, 0.079185 },   // TibiaL2ImpAngZ,
    { 2.642456, 0.697337, 0.021827 },    // TibiaL3ImpAngX,
    { 2.590692, 0.890522, 0.021827 },    // TibiaL3ImpAngY,
    { 2.695698, 0.815130, 0.079185 },    // TibiaL3ImpAngZ,
    { 2.222172, 1.998443, 0.021827 },    // TibiaL4ImpAngX,
    { 2.093614, 2.151651, 0.021827 },    // TibiaL4ImpAngY,
    { 2.220643, 2.127701, 0.079185 },    // TibiaL4ImpAngZ,
    { -2.337326, -1.833986, 0.021827 },  // TibiaR1ImpAngX,
    { -2.437325, -1.660781, 0.021827 },  // TibiaR1ImpAngY,
    { -2.458266, -1.788341, 0.079185 },  // TibiaR1ImpAngZ,
    { -2.642538, -0.696963, 0.021827 },  // TibiaR2ImpAngX,
    { -2.677268, -0.500001, 0.021827 },  // TibiaR2ImpAngY,
    { -2.740574, -0.612706, 0.079185 },  // TibiaR2ImpAngZ,
    { -2.642456, 0.697337, 0.021827 },   // TibiaR3ImpAngX,
    { -2.590692, 0.890522, 0.021827 },   // TibiaR3ImpAngY,
    { -2.695698, 0.815130, 0.079185 },   // TibiaR3ImpAngZ,
    { -2.222172, 1.998443, 0.021827 },   // TibiaR4ImpAngX,
    { -2.093614, 2.151651, 0.021827 },   // TibiaR4ImpAngY,
    { -2.220643, 2.127701, 0.079185 },   // TibiaR4ImpAngZ,
    { 0.453923, -0.746603, -0.000000 },  // TrochanterL1ImpAngX,
    { 0.553923, -0.573397, -0.000000 },  // TrochanterL1ImpAngY,
    { 0.503923, -0.660000, 0.100000 },   // TrochanterL1ImpAngZ,
    { 0.500812, -0.319319, 0.000000 },   // TrochanterL2ImpAngX,
    { 0.535542, -0.122357, 0.000000 },   // TrochanterL2ImpAngY,
    { 0.518177, -0.220838, 0.100000 },   // TrochanterL2ImpAngZ,
    { 0.541793, 0.134466, 0.000000 },    // TrochanterL3ImpAngX,
    { 0.490029, 0.327651, 0.000000 },    // TrochanterL3ImpAngY,
    { 0.515911, 0.231058, 0.100000 },    // TrochanterL3ImpAngZ,
    { 0.556204, 0.600530, -0.000000 },   // TrochanterL4ImpAngX,
    { 0.427647, 0.753739, -0.000000 },   // TrochanterL4ImpAngY,
    { 0.491925, 0.677134, 0.100000 },    // TrochanterL4ImpAngZ,
    { -0.453923, -0.746603, 0.000000 },  // TrochanterR1ImpAngX,
    { -0.553923, -0.573398, 0.000000 },  // TrochanterR1ImpAngY,
    { -0.503923, -0.660000, 0.100000 },  // TrochanterR1ImpAngZ,
    { -0.500812, -0.319319, 0.000000 },  // TrochanterR2ImpAngX,
    { -0.535542, -0.122357, 0.000000 },  // TrochanterR2ImpAngY,
    { -0.518177, -0.220838, 0.100000 },  // TrochanterR2ImpAngZ,
    { -0.541793, 0.134466, 0.000000 },   // TrochanterR3ImpAngX,
    { -0.490029, 0.327651, 0.000000 },   // TrochanterR3ImpAngY,
    { -0.515911, 0.231058, 0.100000 },   // TrochanterR3ImpAngZ,
    { -0.556204, 0.600530, 0.000000 },   // TrochanterR4ImpAngX,
    { -0.427647, 0.753739, 0.000000 },   // TrochanterR4ImpAngY,
    { -0.491925, 0.677134, 0.100000 },   // TrochanterR4ImpAngZ
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{
    // { 0.000000, 1.565926, 0.258819 },    // AbdominOutput,
    // { 0.000000, -0.850000, 0.446689 },   // EyeOutput,
    { 1.008661, -0.951411, 0.324071 },   // FemurL1Output,
    { 1.092143, -0.322044, 0.324071 },   // FemurL2Output,
    { 1.078873, 0.381903, 0.324071 },    // FemurL3Output,
    { 0.938392, 1.051764, 0.324071 },    // FemurL4Output,
    { -1.008661, -0.951411, 0.324071 },  // FemurR1Output,
    { -1.092143, -0.322044, 0.324071 },  // FemurR2Output,
    { -1.078873, 0.381903, 0.324071 },   // FemurR3Output,
    { -0.938392, 1.051765, 0.324071 },   // FemurR4Output,
    // { 0.000000, 0.722260, 0.032758 },    // HipOutput,
    // { -0.000000, -0.709850, 0.109850 },  // NeckOutput,
    { 1.750623, -1.379782, 0.578682 },   // PatellaL1Output,
    { 1.935871, -0.470816, 0.578682 },   // PatellaL2Output,
    { 1.906424, 0.603645, 0.578682 },    // PatellaL3Output,
    { 1.594696, 1.602469, 0.578682 },    // PatellaL4Output,
    { -1.750623, -1.379782, 0.578682 },  // PatellaR1Output,
    { -1.935871, -0.470816, 0.578682 },  // PatellaR2Output,
    { -1.906424, 0.603645, 0.578682 },   // PatellaR3Output,
    { -1.594696, 1.602469, 0.578682 },   // PatellaR4Output,
    // { 2.765063, -1.965469, -0.720684 },  // TarsusL1Output,
    // { 3.089452, -0.674223, -0.720687 },  // TarsusL2Output,
    // { 3.037887, 0.906820, -0.720687 },   // TarsusL3Output,
    // { 2.492023, 2.355415, -0.720687 },   // TarsusL4Output,
    // { -2.765065, -1.965471, -0.720687 }, // TarsusR1Output,
    // { -3.089452, -0.674223, -0.720687 }, // TarsusR2Output,
    // { -3.037887, 0.906820, -0.720687 },  // TarsusR3Output,
    // { -2.492023, 2.355415, -0.720687 },  // TarsusR4Output,
    { 2.387326, -1.747383, 0.021827 },   // TibiaL1Output,
    { 2.659903, -0.598482, 0.021827 },   // TibiaL2Output,
    { 2.616574, 0.793929, 0.021827 },    // TibiaL3Output,
    { 2.157893, 2.075047, 0.021827 },    // TibiaL4Output,
    { -2.387326, -1.747383, 0.021827 },  // TibiaR1Output,
    { -2.659903, -0.598482, 0.021827 },  // TibiaR2Output,
    { -2.616574, 0.793929, 0.021827 },   // TibiaR3Output,
    { -2.157893, 2.075047, 0.021827 },   // TibiaR4Output,
    { 0.503923, -0.660000, -0.000000 },  // TrochanterL1Output,
    { 0.518177, -0.220838, 0.000000 },   // TrochanterL2Output,
    { 0.515911, 0.231058, 0.000000 },    // TrochanterL3Output,
    { 0.491925, 0.677134, -0.000000 },   // TrochanterL4Output,
    { -0.503923, -0.660000, 0.000000 },  // TrochanterR1Output,
    { -0.518177, -0.220838, 0.000000 },  // TrochanterR2Output,
    { -0.515911, 0.231058, 0.000000 },   // TrochanterR3Output,
    { -0.491925, 0.677134, 0.000000 },   // TrochanterR4Output
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
    NEAT::ActivationFunction::UNSIGNED_SIGMOID;

  mSubstrate->m_with_distance = true;

  mSubstrate->m_max_weight_and_bias = 8.0;
}
void SpiderSwarm::setDefaultPopulation() {

  // sanity checking
  if (mParameters == nullptr)
    throw std::runtime_error("Parameters is not set");
  if (mSubstrate == nullptr)
    throw std::runtime_error("Substrate is not set");
  if (mPopulation != nullptr)
    delete mPopulation;

  NEAT::Genome genome(0,
                      mSubstrate->GetMinCPPNInputs(),
                      0,
                      mSubstrate->GetMinCPPNOutputs(),
                      false,
                      NEAT::ActivationFunction::TANH,
                      NEAT::ActivationFunction::TANH,
                      0,
                      *mParameters);

  mPopulation = new NEAT::Population(genome, *mParameters, true, 1.0, time(0));
}
