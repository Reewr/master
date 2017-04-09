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
  size_t gridIndex     = 0;
  size_t gridSize      = grid.size();

  switch (mDrawingMethod) {
    case DrawingMethod::SpeciesLeaders: {
      for (auto& a : mSpeciesLeaders) {
        if (a < numPhenotypes && gridIndex < gridSize) {
          mPhenotypes[a].spider->enableUpdatingFromPhysics();
          mPhenotypes[a].spider->draw(prog, grid[gridIndex], bindTexture);

          if (bindTexture)
            mPhenotypes[a].drawablePhenotype->draw3D(grid[gridIndex] +
                                                     mmm::vec3(0, 5, 0));
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
          mPhenotypes[mBestIndex].drawablePhenotype->draw3D(grid[gridIndex] +
                                                            mmm::vec3(0, 5, 0));
      }
      break;
    case DrawingMethod::DrawAll:
      for (auto& p : mPhenotypes) {
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

  for (auto& i : mPhenotypes) {
    i.drawablePhenotype->recreate(*i.network, mmm::vec3(1.0, 1.0, 1.0));
  }

  mLog->debug("Created {} spiders", mPhenotypes.size());
}

void SpiderSwarm::setDefaultParameters() {
  if (mParameters != nullptr)
    delete mParameters;

  mParameters = new NEAT::Parameters();

  // Basic parameters
  mParameters->PopulationSize       = 50;
  mParameters->DynamicCompatibility = true;
  mParameters->MinSpecies           = 5;
  mParameters->MaxSpecies           = 10;
  mParameters->AllowClones          = true;

  // GA parameters
  mParameters->YoungAgeTreshold                       = 15;
  mParameters->YoungAgeFitnessBoost                   = 1.1;
  mParameters->SpeciesMaxStagnation                   = 15;
  mParameters->StagnationDelta                        = 0.0;
  mParameters->OldAgeTreshold                         = 35;
  mParameters->OldAgePenalty                          = 1.0;
  mParameters->DetectCompetetiveCoevolutionStagnation = false;
  mParameters->KillWorstSpeciesEach                   = 15;
  mParameters->KillWorstAge                           = 10;
  mParameters->SurvivalRate                           = 0.2;
  mParameters->CrossoverRate                          = 0.75;
  mParameters->OverallMutationRate                    = 0.2;
  mParameters->InterspeciesCrossoverRate              = 0.001;
  mParameters->MultipointCrossoverRate                = 0.4;
  mParameters->RouletteWheelSelection                 = false;
  mParameters->TournamentSize                         = 4;
  mParameters->EliteFraction                          = 0.15;

  // Mutation parameters
  mParameters->MutateAddNeuronProb       = 0.1;
  mParameters->SplitRecurrent            = true;
  mParameters->SplitLoopedRecurrent      = true;
  mParameters->MutateAddLinkProb         = 0.2;
  mParameters->MutateAddLinkFromBiasProb = 0.0;
  mParameters->MutateRemLinkProb         = 0.0;
  mParameters->MutateRemSimpleNeuronProb = 0.0;
  mParameters->LinkTries                 = 32;
  mParameters->RecurrentProb             = 0.50;
  mParameters->RecurrentLoopProb         = 0.25;

  // Parameter mutation parameters
  mParameters->MutateWeightsProb             = 0.8;
  mParameters->MutateWeightsSevereProb       = 0.5;
  mParameters->WeightMutationRate            = 0.25;
  mParameters->WeightMutationMaxPower        = 0.5;
  mParameters->WeightReplacementMaxPower     = 1.0;
  mParameters->MaxWeight                     = 8.0;
  mParameters->MutateActivationAProb         = 0.0;
  mParameters->MutateActivationBProb         = 0.0;
  mParameters->ActivationAMutationMaxPower   = 0.5;
  mParameters->ActivationBMutationMaxPower   = 0.0;
  mParameters->MinActivationA                = 1.1;
  mParameters->MaxActivationA                = 6.9;
  mParameters->MinActivationB                = 0.0;
  mParameters->MaxActivationB                = 0.0;
  mParameters->TimeConstantMutationMaxPower  = 0.1;
  mParameters->MutateNeuronTimeConstantsProb = 0.0;
  mParameters->MutateNeuronBiasesProb        = 0.1;
  mParameters->MinNeuronTimeConstant         = 0.04;
  mParameters->MaxNeuronTimeConstant         = 0.24;
  mParameters->MinNeuronBias                 = -8.0;
  mParameters->MaxNeuronBias                 = 8.0;

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
  mParameters->DisjointCoeff                         = 1.0;
  mParameters->ExcessCoeff                           = 1.0;
  mParameters->WeightDiffCoeff                       = 1.0;
  mParameters->ActivationADiffCoeff                  = 0.0;
  mParameters->ActivationBDiffCoeff                  = 0.0;
  mParameters->TimeConstantDiffCoeff                 = 0.0;
  mParameters->BiasDiffCoeff                         = 0.0;
  mParameters->ActivationFunctionDiffCoeff           = 0.0;
  mParameters->CompatTreshold                        = 2.0;
  mParameters->MinCompatTreshold                     = 0.2;
  mParameters->CompatTresholdModifier                = 0.3;
  mParameters->CompatTreshChangeInterval_Generations = 1;
  mParameters->CompatTreshChangeInterval_Evaluations = 10;

  // ES-HyperNEAT parameters
  mParameters->DivisionThreshold = 0.03;
  mParameters->VarianceThreshold = 0.03;
  mParameters->BandThreshold     = 0.3;
  mParameters->InitialDepth      = 3;
  mParameters->MaxDepth          = 4;
  mParameters->IterationLevel    = 1;
  mParameters->CPPN_Bias         = 1.0;
  mParameters->Width             = 2.0;
  mParameters->Height            = 2.0;
  mParameters->Qtree_X           = 0.0;
  mParameters->Qtree_Y           = 0.0;
  mParameters->Leo               = false;
  mParameters->LeoThreshold      = 0.3;
  mParameters->LeoSeed           = false;
  mParameters->GeometrySeed      = false;
}
void SpiderSwarm::setDefaultSubstrate() {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  // clang-format off
  std::vector<std::vector<double>> inputs{
    { 0.000000, 0.000000, 0.000000 },    // Phase,
    { 2.841038, -2.009333, -1.048089 },  // L1Tip
    { 3.175848, -0.689457, -1.048092 },  // L2Tip
    { 3.122624, 0.929526, -1.048092 },   // L3Tip
    { 2.559226, 2.411806, -1.048092 },   // L4Tip
    { -2.841039, -2.009335, -1.048092 }, // R1Tip
    { -3.175846, -0.689457, -1.048092 }, // R2Tip
    { -3.122626, 0.929525, -1.048092 },  // R3Tip
    { -2.559227, 2.411805, -1.048092 },  // R4Tip
    { 0.000000, 0.898278, 0.079924 },    // AbdominImpRot
    { 0.000000, -0.850000, 0.242041 },   // EyeImpRot
    { 0.607846, -0.720000, -0.000000 },  // FemurL1ImpRot
    { 0.636354, -0.241676, 0.000000 },   // FemurL2ImpRot
    { 0.631822, 0.262117, 0.000000 },    // FemurL3ImpRot
    { 0.583851, 0.754269, 0.000000 },    // FemurL4ImpRot
    { -0.607846, -0.720000, 0.000000 },  // FemurR1ImpRot
    { -0.636354, -0.241676, 0.000000 },  // FemurR2ImpRot
    { -0.631822, 0.262117, 0.000000 },   // FemurR3ImpRot
    { -0.583851, 0.754269, 0.000000 },   // FemurR4ImpRot
    { 0.000000, 0.600004, 0.000000 },    // HipImpRot
    { 0.000000, -0.599997, -0.000003 },  // NeckImpRot
    { 1.409475, -1.182821, 0.648141 },   // PatellaL1ImpRot
    { 1.547933, -0.402412, 0.648141 },   // PatellaL2ImpRot
    { 1.525923, 0.501690, 0.648141 },    // PatellaL3ImpRot
    { 1.292933, 1.349260, 0.648141 },    // PatellaL4ImpRot
    { -1.409475, -1.182821, 0.648141 },  // PatellaR1ImpRot
    { -1.547933, -0.402412, 0.648141 },  // PatellaR2ImpRot
    { -1.525923, 0.501690, 0.648141 },   // PatellaR3ImpRot
    { -1.292933, 1.349260, 0.648141 },   // PatellaR4ImpRot
    { 2.707002, -1.931948, -0.470476 },  // TarsusL1ImpRot
    { 3.023428, -0.662581, -0.470480 },  // TarsusL2ImpRot
    { 2.973128, 0.889468, -0.470480 },   // TarsusL3ImpRot
    { 2.440665, 2.312321, -0.470480 },   // TarsusL4ImpRot
    { -2.707004, -1.931949, -0.470480 }, // TarsusR1ImpRot
    { -3.023428, -0.662581, -0.470480 }, // TarsusR2ImpRot
    { -2.973128, 0.889468, -0.470480 },  // TarsusR3ImpRot
    { -2.440665, 2.312321, -0.470480 },  // TarsusR4ImpRot
    { 2.091770, -1.576744, 0.509223 },   // TibiaL1ImpRot
    { 2.323810, -0.539220, 0.509223 },   // TibiaL2ImpRot
    { 2.286924, 0.705600, 0.509223 },    // TibiaL3ImpRot
    { 1.896459, 1.855678, 0.509223 },    // TibiaL4ImpRot
    { -2.091770, -1.576744, 0.509223 },  // TibiaR1ImpRot
    { -2.323810, -0.539220, 0.509223 },  // TibiaR2ImpRot
    { -2.286924, 0.705600, 0.509223 },   // TibiaR3ImpRot
    { -1.896459, 1.855678, 0.509223 },   // TibiaR4ImpRot
    { 0.400000, -0.600000, -0.000000 },  // TrochanterL1ImpRot
    { 0.400000, -0.200000, 0.000000 },   // TrochanterL2ImpRot
    { 0.400000, 0.200000, 0.000000 },    // TrochanterL3ImpRot
    { 0.400000, 0.600000, -0.000000 },   // TrochanterL4ImpRot
    { -0.400000, -0.600000, 0.000000 },  // TrochanterR1ImpRot
    { -0.400000, -0.200000, 0.000000 },  // TrochanterR2ImpRot
    { -0.400000, 0.200000, 0.000000 },   // TrochanterR3ImpRot
    { -0.400000, 0.600000, 0.000000 }    // TrochanterR4ImpRot
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
