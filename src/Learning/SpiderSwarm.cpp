#include "SpiderSwarm.hpp"

#include "../3D/World.hpp"
#include "../3D/Spider.hpp"

#include <btBulletDynamicsCommon.h>

#include <Genome.h>
#include <NeuralNetwork.h>
#include <Parameters.h>
#include <Population.h>
#include <Substrate.h>

/**
 * @brief
 *   For simulation purposes, the spiders should not collide with each other
 *   as they are technically suppose to live in their own world.
 *
 *   In order to save memory, we add all the spiders to the same world, but
 *   use this function to ignore any collision between spiders.
 *
 *   By default, only filter group and mask is taken into consideration
 *   when evaluating whether or not to perform collision detection on an object.
 *
 *   In addition to the default behaviour, the function also checks to see if
 *   the rigid bodies have the same parent. If this is the case, normal
 *   collision detection is performed. If they do not have the same parent,
 *   they should never be able to collide
 *
 * @param a
 * @param b
 *
 * @return
 */
bool SpiderSwarm::NonSpiderCollisionFilter::needBroadphaseCollision (
    btBroadphaseProxy* a,
    btBroadphaseProxy* b) const {
  bool collides = (a->m_collisionFilterGroup & b->m_collisionFilterMask) != 0 &&
                  (b->m_collisionFilterGroup & a->m_collisionFilterMask);

  // If collides is false, dont check anything else
  if (!collides)
    return false;

  btCollisionObject* aObj = (btCollisionObject*) a->m_clientObject;
  btCollisionObject* bObj = (btCollisionObject*) b->m_clientObject;

  if (!aObj || !bObj) {
    return true;
  }

  int aId = aObj->getUserIndex();
  int bId = bObj->getUserIndex();

  if (aId == -1 || bId == -1) {
    return true;
  }

  if (aId == bId) {
    return true;
  }

  return false;
}

SpiderSwarm::Phenotype::Phenotype()
  : world(nullptr)
  , spider(nullptr)
  , network(nullptr)
  , fitness(0) {}

SpiderSwarm::Phenotype::~Phenotype() {
  if (world != nullptr)
    delete world;
  if (spider != nullptr)
    delete spider;
  if (network != nullptr)
    delete network;
}


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

// SpiderSwarm::SpiderSwarm(NEAT::Parameters* p, NEAT::Substrate* s, )
//     : mParameters(p)
//     , mSubstrate(s) {}

/**
 * @brief
 *   Deletes all the spiders after detaching them from the world.
 */
SpiderSwarm::~SpiderSwarm() {
  mPhenotypes.clear();
  delete mParameters;
  delete mSubstrate;
  delete mPopulation;
}

/**
 * @brief
 *   Goes through all the stored spiders and worlds and updates them
 */
void SpiderSwarm::update(float deltaTime) {
  if (mCurrentDuration == 0)
    mLog->debug("Processing {} individuals", mBatchEnd - mBatchStart);

  if (mCurrentDuration < mIterationDuration) {
    updateNormal(deltaTime);
  } else if (mBatchEnd < mPhenotypes.size()) {
    updateBatch();
  } else {
    updateEpoch();
  }
}

void SpiderSwarm::draw(std::shared_ptr<Program>& prog, bool bindTexture) {
  size_t draw = mmm::min(mBatchEnd - mBatchStart, mDrawLimit) + mBatchStart;
  for (size_t i = mBatchStart; i < draw; ++i) {
    mPhenotypes[i].spider->draw(prog, bindTexture);
  }
}

void SpiderSwarm::updateNormal(float deltaTime) {
  for (size_t i = mBatchStart; i < mBatchEnd; ++i) {

    // activate network for each spider given current motion state

    mPhenotypes[i].world->doPhysics(deltaTime);

    // update fitness if we can detect some state that we can judge fitness on

  }

  mCurrentDuration += deltaTime;
}
void SpiderSwarm::updateBatch() {
  mLog->debug("Complete batch {}", mCurrentBatch);

  mCurrentDuration = 0;
  mCurrentBatch += 1;

  mBatchStart = mCurrentBatch * mBatchSize;
  mBatchEnd   = mmm::min((mCurrentBatch + 1) * mBatchSize, mPhenotypes.size());
}
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

  mPhenotypes.clear();
  mPhenotypes.resize(mParameters->PopulationSize);

  size_t index = 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    for (size_t j = 0; j < mPopulation->m_Species[i].m_Individuals.size(); ++j) {
      mPhenotypes[index].spider  = new Spider();
      mPhenotypes[index].world   = new World(mmm::vec3(0, -9.81, 0));
      mPhenotypes[index].network = new NEAT::NeuralNetwork();

      mPopulation->m_Species[i].m_Individuals[j].BuildESHyperNEATPhenotype(
        *mPhenotypes[index].network, *mSubstrate, *mParameters);

      mPhenotypes[index].world->addObject(mPhenotypes[index].spider);
      index += 1;
    }
  }

  mLog->debug("Created {} spiders", index);
}

void SpiderSwarm::setDefaultParameters() {
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
