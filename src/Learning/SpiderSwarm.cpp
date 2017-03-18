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

int SpiderSwarm::mBodyIds = 0;

SpiderSwarm::SpiderSwarm()
    : mParameters(nullptr)
    , mSubstrate(nullptr)
    , mPopulation(nullptr) {

  setDefualtParameters();
  setDefaultSubstrate();
  setDefaultPopulation();
}

// SpiderSwarm::SpiderSwarm(NEAT::Parameters* p, NEAT::Substrate* s, )
//     : mParameters(p)
//     , mSubstrate(s) {}

/**
 * @brief
 *   Deletes all the spiders after detaching them from the world.
 */
SpiderSwarm::~SpiderSwarm() {
  for (auto& c : mSpiders) {
    removeSpider(c.first);
  }

  mSpiders.clear();
  delete mParameters;
  delete mSubstrate;
}

/**
 * @brief
 *   Adds a new spider to the list of spiders as well as to the world. Each
 *   added spider gets a unique Id that is added to each of the rigid bodies.
 *   In addition, each child of spider gets a pointer to the spider object.
 *
 * @return
 */
Spider* SpiderSwarm::addSpider() {
  int     id      = ++mBodyIds;
  Spider* spider  = new Spider();
  World*  world   = new World(mmm::vec3(0, -9.81, 0));

  NEAT::NeuralNetwork* network = new NEAT::NeuralNetwork();

  mSpiders[id] = {world, spider, network, mmm::vec<8>(0) };

  for (auto& child : spider->children()) {
    child->rigidBody()->setUserIndex(id);
  }

  world->addObject(spider);

  return spider;
}

/**
 * @brief
 *   Removes a spider indicated by an id, removing it
 *   from storage, the world and deleting the allocated object
 *
 * @param id
 *
 * @return
 */
bool SpiderSwarm::removeSpider(int id) {
  if (id < 0 || mSpiders.count(id))
    return false;

  mSpiders[id].world->removeObject(mSpiders[id].spider);

  delete mSpiders[id].spider;
  delete mSpiders[id].world;
  delete mSpiders[id].network;

  return true;
}

/**
 * @brief
 *   Retrieves a spider by id, returning a nullptr if it does not exist
 *   otherwise returning a pointer to the spider
 *
 * @param id
 *
 * @return
 */
Spider* SpiderSwarm::spider(int id) {
  if (!mSpiders.count(id))
    return nullptr;
  return mSpiders[id].spider;
}

/**
 * @brief
 *   Returns a const reference to all the spiders
 *
 * @return
 */
const std::map<int, SpiderSwarm::Phenotype>& SpiderSwarm::spiders() {
  return mSpiders;
}

/**
 * @brief
 *   Goes through all the stored spiders and worlds and updates them
 */
void SpiderSwarm::update(float deltaTime) {
  for(auto& spiderWorld : mSpiders) {
    spiderWorld.second.world->doPhysics(deltaTime);
  }
}

void SpiderSwarm::setDefualtParameters() {
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
                      mSubstrate->GetOutputs(),
                      false,
                      NEAT::ActivationFunction::TANH,
                      NEAT::ActivationFunction::TANH,
                      0,
                      *mParameters);

  mPopulation = new NEAT::Population(genome, *mParameters, true, 1.0, time(0));
}
