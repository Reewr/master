#include "SpiderSwarm.hpp"

#include "../3D/World.hpp"
#include "../3D/Spider.hpp"

#include <btBulletDynamicsCommon.h>

#include <Genome.h>
#include <NeuralNetwork.h>
#include <Parameters.h>
#include <Population.h>
#include <Substrate.h>

SpiderSwarm::SpiderSwarm()
    : Logging::Log("SpiderSwarm")
    , mCurrentBatch(0)
    , mBatchStart(0)
    , mBatchEnd(10)
    , mBatchSize(10)
    , mCurrentDuration(0)
    , mIterationDuration(10)
    , mDrawLimit(1)
    , mParameters(nullptr)
    , mSubstrate(nullptr)
    , mPopulation(nullptr) {

  setDefaultParameters();
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
  mPhenotypes.clear();
  delete mParameters;
  delete mSubstrate;
  delete mPopulation;
}

/**
 * @brief
 *
 */
void SpiderSwarm::update(float deltaTime) {
  if (mCurrentDuration == 0)
    mLog->debug("Processing {} individuals", mBatchEnd - mBatchStart);

  if (mCurrentDuration < mIterationDuration) {
    updateNormal(deltaTime);
  } else if (mBatchEnd < mPhenotypes.size()) {
    setNextBatch();
  } else {
    updateEpoch();
  }
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
  size_t draw = mmm::min(mBatchEnd - mBatchStart, mDrawLimit) + mBatchStart;
  for (size_t i = mBatchStart; i < draw; ++i) {
    mPhenotypes[i].spider->draw(prog, bindTexture);
  }
}

/**
 * @brief
 *   Goes through the current patch and performs the following on it:
 *
 *   1. Activate each network and use output to set information on the spider
 *   2. Perform physics to see the changes of the given output
 *   3. Update fitness based on how well it did.
 *
 * @param deltaTime
 */
void SpiderSwarm::updateNormal(float deltaTime) {
  for (size_t i = mBatchStart; i < mBatchEnd; ++i) {
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

  for (size_t i = 0; i < mPopulation->m_Species.size(); i++) {
    for (size_t j = 0; j < mPopulation->m_Species[i].m_Individuals.size(); j++) {

      // calculate total fitness for each individual

      mPopulation->m_Species[i].m_Individuals[j].SetFitness(0.0);
      mPopulation->m_Species[i].m_Individuals[j].SetEvaluated();
    }
  }

  mPopulation->Epoch();
  recreatePhenotypes();
}

void SpiderSwarm::recreatePhenotypes() {
  mLog->debug("Recreating {} phenotypes...", mParameters->PopulationSize);

  mPhenotypes.resize(mParameters->PopulationSize);

  size_t index = 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    auto& species = mPopulation->m_Species[i];

    for (size_t j = 0; j < species.m_Individuals.size(); ++j) {
      auto& individual = species.m_Individuals[j];

      mPhenotypes[index].reset();
      individual.BuildESHyperNEATPhenotype(*mPhenotypes[index].network,
                                           *mSubstrate,
                                           *mParameters);
      index += 1;
    }
  }

  mLog->debug("Created {} spiders", index);
}

void SpiderSwarm::setDefaultParameters() {
  if (mParameters != nullptr)
    delete mParameters;

  mParameters = new NEAT::Parameters();

  mParameters->PopulationSize = 50;

  mParameters->DynamicCompatibility = true;
  mParameters->CompatTreshold = 2.0;
  mParameters->YoungAgeTreshold = 15;
  mParameters->SpeciesMaxStagnation = 100;
  mParameters->OldAgeTreshold = 35;
  mParameters->MinSpecies = 5;
  mParameters->MaxSpecies = 10;
  mParameters->RouletteWheelSelection = false;

  mParameters->MutateRemLinkProb = 0.02;
  mParameters->RecurrentProb = 0;
  mParameters->OverallMutationRate = 0.15;
  mParameters->MutateAddLinkProb = 0.08;
  mParameters->MutateAddNeuronProb = 0.01;
  mParameters->MutateWeightsProb = 0.90;
  mParameters->MaxWeight = 8.0;
  mParameters->WeightMutationMaxPower = 0.2;
  mParameters->WeightReplacementMaxPower = 1.0;

  mParameters->MutateActivationAProb = 0.0;
  mParameters->ActivationAMutationMaxPower = 0.5;
  mParameters->MinActivationA = 0.05;
  mParameters->MaxActivationA = 6.0;

  mParameters->MutateNeuronActivationTypeProb = 0.03;

  mParameters->ActivationFunction_SignedSigmoid_Prob = 0.0;
  mParameters->ActivationFunction_UnsignedSigmoid_Prob = 0.0;
  mParameters->ActivationFunction_Tanh_Prob = 1.0;
  mParameters->ActivationFunction_TanhCubic_Prob = 0.0;
  mParameters->ActivationFunction_SignedStep_Prob = 1.0;
  mParameters->ActivationFunction_UnsignedStep_Prob = 0.0;
  mParameters->ActivationFunction_SignedGauss_Prob = 1.0;
  mParameters->ActivationFunction_UnsignedGauss_Prob = 0.0;
  mParameters->ActivationFunction_Abs_Prob = 0.0;
  mParameters->ActivationFunction_SignedSine_Prob = 1.0;
  mParameters->ActivationFunction_UnsignedSine_Prob = 0.0;
  mParameters->ActivationFunction_Linear_Prob = 1.0;

  mParameters->DivisionThreshold = 0.5;
  mParameters->VarianceThreshold = 0.03;
  mParameters->BandThreshold = 0.3;
  mParameters->InitialDepth = 2;
  mParameters->MaxDepth = 3;
  mParameters->IterationLevel = 1;
  mParameters->Leo = false;
  mParameters->GeometrySeed = false;
  mParameters->LeoSeed = false;
  mParameters->LeoThreshold = 0.3;
  mParameters->CPPN_Bias = -1.0;
  mParameters->Qtree_X = 0.0;
  mParameters->Qtree_Y = 0.0;
  mParameters->Width = 1.0;
  mParameters->Height = 1.0;
  mParameters->EliteFraction = 0.1;
}
void SpiderSwarm::setDefaultSubstrate() {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  std::vector<std::vector<double>> inputs{
    { -1.0, -1.0, 0.0 },
    { 1.0, -1.0, 0.0 },
    { 0.0, -1.0, 0.0 }
  };

  std::vector<std::vector<double>> hidden{};

  std::vector<std::vector<double>> outputs{
    {0.0, 1.0, 0.0}
  };

  mSubstrate = new NEAT::Substrate(inputs, hidden, outputs);

  mSubstrate->m_allow_input_hidden_links = false;
  mSubstrate->m_allow_input_output_links = false;
  mSubstrate->m_allow_hidden_hidden_links = false;
  mSubstrate->m_allow_hidden_output_links = false;
  mSubstrate->m_allow_output_hidden_links = false;
  mSubstrate->m_allow_output_output_links = false;
  mSubstrate->m_allow_looped_hidden_links = false;
  mSubstrate->m_allow_looped_output_links = false;

  mSubstrate->m_allow_input_hidden_links = true;
  mSubstrate->m_allow_input_output_links = false;
  mSubstrate->m_allow_hidden_output_links = true;
  mSubstrate->m_allow_hidden_hidden_links = false;

  mSubstrate->m_hidden_nodes_activation =
    NEAT::ActivationFunction::SIGNED_SIGMOID;
  mSubstrate->m_output_nodes_activation =
    NEAT::ActivationFunction::UNSIGNED_SIGMOID;

  mSubstrate->m_with_distance = false;

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
