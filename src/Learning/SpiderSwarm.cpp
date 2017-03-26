#include "SpiderSwarm.hpp"

#include "../3D/Spider.hpp"
#include "../3D/World.hpp"

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
    , mBatchEnd(7)
    , mBatchSize(7)
    , mCurrentDuration(0)
    , mIterationDuration(5)
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

  float      best       = -1.f;
  static int generation = 1;

  size_t index = 0;
  for (size_t i = 0; i < mPopulation->m_Species.size(); ++i) {
    for (size_t j = 0; j < mPopulation->m_Species[i].m_Individuals.size(); ++j) {

      float fitness = mmm::product(mPhenotypes[index].fitness);
      mPopulation->m_Species[i].m_Individuals[j].SetFitness(fitness);
      mPopulation->m_Species[i].m_Individuals[j].SetEvaluated();

      best = mmm::max(fitness, best);
      index += 1;
    }
  }

  mLog->info("Generation {}: best {}", generation, best);
  generation += 1;

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

  mParameters->DynamicCompatibility   = true;
  mParameters->CompatTreshold         = 2.0;
  mParameters->YoungAgeTreshold       = 15;
  mParameters->SpeciesMaxStagnation   = 100;
  mParameters->OldAgeTreshold         = 35;
  mParameters->MinSpecies             = 5;
  mParameters->MaxSpecies             = 10;
  mParameters->RouletteWheelSelection = false;

  mParameters->MutateRemLinkProb         = 0.02;
  mParameters->RecurrentProb             = 0;
  mParameters->OverallMutationRate       = 0.15;
  mParameters->MutateAddLinkProb         = 0.08;
  mParameters->MutateAddNeuronProb       = 0.01;
  mParameters->MutateWeightsProb         = 0.90;
  mParameters->MaxWeight                 = 8.0;
  mParameters->WeightMutationMaxPower    = 0.2;
  mParameters->WeightReplacementMaxPower = 1.0;

  mParameters->MutateActivationAProb       = 0.0;
  mParameters->ActivationAMutationMaxPower = 0.5;
  mParameters->MinActivationA              = 0.05;
  mParameters->MaxActivationA              = 6.0;

  mParameters->MutateNeuronActivationTypeProb = 0.03;

  mParameters->ActivationFunction_SignedSigmoid_Prob   = 0.0;
  mParameters->ActivationFunction_UnsignedSigmoid_Prob = 0.0;
  mParameters->ActivationFunction_Tanh_Prob            = 1.0;
  mParameters->ActivationFunction_TanhCubic_Prob       = 0.0;
  mParameters->ActivationFunction_SignedStep_Prob      = 1.0;
  mParameters->ActivationFunction_UnsignedStep_Prob    = 0.0;
  mParameters->ActivationFunction_SignedGauss_Prob     = 1.0;
  mParameters->ActivationFunction_UnsignedGauss_Prob   = 0.0;
  mParameters->ActivationFunction_Abs_Prob             = 0.0;
  mParameters->ActivationFunction_SignedSine_Prob      = 1.0;
  mParameters->ActivationFunction_UnsignedSine_Prob    = 0.0;
  mParameters->ActivationFunction_Linear_Prob          = 1.0;

  mParameters->DivisionThreshold = 0.5;
  mParameters->VarianceThreshold = 0.03;
  mParameters->BandThreshold     = 0.3;
  mParameters->InitialDepth      = 2;
  mParameters->MaxDepth          = 3;
  mParameters->IterationLevel    = 1;
  mParameters->Leo               = false;
  mParameters->GeometrySeed      = false;
  mParameters->LeoSeed           = false;
  mParameters->LeoThreshold      = 0.3;
  mParameters->CPPN_Bias         = -1.0;
  mParameters->Qtree_X           = 0.0;
  mParameters->Qtree_Y           = 0.0;
  mParameters->Width             = 1.0;
  mParameters->Height            = 1.0;
  mParameters->EliteFraction     = 0.1;
}
void SpiderSwarm::setDefaultSubstrate() {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  std::vector<std::vector<double>> inputs{
    { -0.100000, 1.565926, 0.258819 },   { 0.100000, 1.565926, 0.258819 },
    { 0.000000, 1.540044, 0.355412 },    { 0.000000, 0.898278, 0.079924 },
    { -0.100000, -0.850000, 0.446689 },  { 0.100000, -0.850000, 0.446689 },
    { 0.000000, -0.850000, 0.546689 },   { 0.000000, -0.850000, 0.242041 },
    { 0.958661, -1.038013, 0.324071 },   { 1.058661, -0.864808, 0.324071 },
    { 0.958988, -0.922732, 0.405986 },   { 0.607846, -0.720000, -0.000000 },
    { 1.074779, -0.420524, 0.324071 },   { 1.109508, -0.223563, 0.324071 },
    { 1.035657, -0.312084, 0.405986 },   { 0.636354, -0.241676, 0.000000 },
    { 1.104755, 0.285311, 0.324071 },    { 1.052991, 0.478496, 0.324071 },
    { 1.023470, 0.367058, 0.405986 },    { 0.631822, 0.262117, 0.000000 },
    { 1.002671, 0.975160, 0.324071 },    { 0.874113, 1.128369, 0.324071 },
    { 0.894454, 1.014896, 0.405986 },    { 0.583851, 0.754269, 0.000000 },
    { -0.958661, -1.038013, 0.324071 },  { -1.058661, -0.864808, 0.324071 },
    { -0.958988, -0.922732, 0.405986 },  { -0.607846, -0.720000, 0.000000 },
    { -1.074779, -0.420524, 0.324071 },  { -1.109508, -0.223563, 0.324071 },
    { -1.035657, -0.312083, 0.405986 },  { -0.636354, -0.241676, 0.000000 },
    { -1.104755, 0.285311, 0.324071 },   { -1.052991, 0.478496, 0.324071 },
    { -1.023470, 0.367058, 0.405986 },   { -0.631822, 0.262117, 0.000000 },
    { -1.002671, 0.975160, 0.324071 },   { -0.874113, 1.128369, 0.324071 },
    { -0.894454, 1.014896, 0.405986 },   { -0.583851, 0.754269, 0.000000 },
    { -0.100000, 0.722260, 0.032758 },   { 0.100000, 0.722260, 0.032758 },
    { 0.000000, 0.696378, 0.129351 },    { 0.000000, 0.600004, 0.000000 },
    { -0.100000, -0.709850, 0.109850 },  { 0.100000, -0.709850, 0.109850 },
    { -0.000000, -0.639139, 0.180561 },  { 0.000000, -0.599997, -0.000003 },
    { 1.700623, -1.466385, 0.578682 },   { 1.800623, -1.293180, 0.578682 },
    { 1.765661, -1.388465, 0.677163 },   { 1.409475, -1.182821, 0.648141 },
    { 1.918507, -0.569296, 0.578682 },   { 1.953236, -0.372335, 0.578682 },
    { 1.952973, -0.473831, 0.677163 },   { 1.547933, -0.402412, 0.648141 },
    { 1.932306, 0.507052, 0.578682 },    { 1.880542, 0.700238, 0.578682 },
    { 1.923197, 0.608139, 0.677163 },    { 1.525923, 0.501690, 0.648141 },
    { 1.658975, 1.525864, 0.578682 },    { 1.530417, 1.679073, 0.578682 },
    { 1.607998, 1.613631, 0.677163 },    { 1.292933, 1.349260, 0.648141 },
    { -1.700623, -1.466385, 0.578682 },  { -1.800623, -1.293180, 0.578682 },
    { -1.765661, -1.388465, 0.677163 },  { -1.409475, -1.182821, 0.648141 },
    { -1.918507, -0.569296, 0.578682 },  { -1.953236, -0.372335, 0.578682 },
    { -1.952972, -0.473831, 0.677163 },  { -1.547933, -0.402412, 0.648141 },
    { -1.932306, 0.507052, 0.578682 },   { -1.880542, 0.700238, 0.578682 },
    { -1.923197, 0.608139, 0.677163 },   { -1.525923, 0.501690, 0.648141 },
    { -1.658975, 1.525864, 0.578682 },   { -1.530417, 1.679073, 0.578682 },
    { -1.607998, 1.613631, 0.677163 },   { -1.292933, 1.349260, 0.648141 },
    { 2.715063, -2.052072, -0.720684 },  { 2.815063, -1.878867, -0.720684 },
    { 2.848714, -2.013766, -0.694802 },  { 2.707002, -1.931948, -0.470476 },
    { 3.072088, -0.772704, -0.720687 },  { 3.106817, -0.575742, -0.720687 },
    { 3.184577, -0.690996, -0.694806 },  { 3.023428, -0.662581, -0.470480 },
    { 3.063769, 0.810227, -0.720687 },   { 3.012005, 1.003412, -0.720687 },
    { 3.131188, 0.931820, -0.694806 },   { 2.973128, 0.889468, -0.470480 },
    { 2.556302, 2.278811, -0.720687 },   { 2.427744, 2.432019, -0.720687 },
    { 2.566017, 2.417504, -0.694806 },   { 2.440665, 2.312321, -0.470480 },
    { -2.715065, -2.052073, -0.720687 }, { -2.815065, -1.878868, -0.720687 },
    { -2.848716, -2.013767, -0.694806 }, { -2.707004, -1.931949, -0.470480 },
    { -3.072088, -0.772704, -0.720687 }, { -3.106817, -0.575742, -0.720687 },
    { -3.184577, -0.690996, -0.694806 }, { -3.023428, -0.662581, -0.470480 },
    { -3.063769, 0.810227, -0.720687 },  { -3.012005, 1.003412, -0.720687 },
    { -3.131188, 0.931820, -0.694806 },  { -2.973128, 0.889468, -0.470480 },
    { -2.556302, 2.278811, -0.720687 },  { -2.427744, 2.432019, -0.720687 },
    { -2.566017, 2.417504, -0.694806 },  { -2.440665, 2.312321, -0.470480 },
    { 2.337326, -1.833986, 0.021827 },   { 2.437326, -1.660781, 0.021827 },
    { 2.458266, -1.788341, 0.079185 },   { 2.091770, -1.576744, 0.509223 },
    { 2.642539, -0.696963, 0.021827 },   { 2.677268, -0.500001, 0.021827 },
    { 2.740574, -0.612706, 0.079185 },   { 2.323810, -0.539220, 0.509223 },
    { 2.642456, 0.697337, 0.021827 },    { 2.590692, 0.890522, 0.021827 },
    { 2.695698, 0.815130, 0.079185 },    { 2.286924, 0.705600, 0.509223 },
    { 2.222172, 1.998443, 0.021827 },    { 2.093614, 2.151651, 0.021827 },
    { 2.220643, 2.127701, 0.079185 },    { 1.896459, 1.855678, 0.509223 },
    { -2.337326, -1.833986, 0.021827 },  { -2.437325, -1.660781, 0.021827 },
    { -2.458266, -1.788341, 0.079185 },  { -2.091770, -1.576744, 0.509223 },
    { -2.642538, -0.696963, 0.021827 },  { -2.677268, -0.500001, 0.021827 },
    { -2.740574, -0.612706, 0.079185 },  { -2.323810, -0.539220, 0.509223 },
    { -2.642456, 0.697337, 0.021827 },   { -2.590692, 0.890522, 0.021827 },
    { -2.695698, 0.815130, 0.079185 },   { -2.286924, 0.705600, 0.509223 },
    { -2.222172, 1.998443, 0.021827 },   { -2.093614, 2.151651, 0.021827 },
    { -2.220643, 2.127701, 0.079185 },   { -1.896459, 1.855678, 0.509223 },
    { 0.453923, -0.746603, -0.000000 },  { 0.553923, -0.573397, -0.000000 },
    { 0.503923, -0.660000, 0.100000 },   { 0.400000, -0.600000, -0.000000 },
    { 0.500812, -0.319319, 0.000000 },   { 0.535542, -0.122357, 0.000000 },
    { 0.518177, -0.220838, 0.100000 },   { 0.400000, -0.200000, 0.000000 },
    { 0.541793, 0.134466, 0.000000 },    { 0.490029, 0.327651, 0.000000 },
    { 0.515911, 0.231058, 0.100000 },    { 0.400000, 0.200000, 0.000000 },
    { 0.556204, 0.600530, -0.000000 },   { 0.427647, 0.753739, -0.000000 },
    { 0.491925, 0.677134, 0.100000 },    { 0.400000, 0.600000, -0.000000 },
    { -0.453923, -0.746603, 0.000000 },  { -0.553923, -0.573398, 0.000000 },
    { -0.503923, -0.660000, 0.100000 },  { -0.400000, -0.600000, 0.000000 },
    { -0.500812, -0.319319, 0.000000 },  { -0.535542, -0.122357, 0.000000 },
    { -0.518177, -0.220838, 0.100000 },  { -0.400000, -0.200000, 0.000000 },
    { -0.541793, 0.134466, 0.000000 },   { -0.490029, 0.327651, 0.000000 },
    { -0.515911, 0.231058, 0.100000 },   { -0.400000, 0.200000, 0.000000 },
    { -0.556204, 0.600530, 0.000000 },   { -0.427647, 0.753739, 0.000000 },
    { -0.491925, 0.677134, 0.100000 },   { -0.400000, 0.600000, 0.000000 }
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{
    { 0.000000, 1.565926, 0.258819 },    { 0.000000, -0.850000, 0.446689 },
    { 1.008661, -0.951411, 0.324071 },   { 1.092143, -0.322044, 0.324071 },
    { 1.078873, 0.381903, 0.324071 },    { 0.938392, 1.051764, 0.324071 },
    { -1.008661, -0.951411, 0.324071 },  { -1.092143, -0.322044, 0.324071 },
    { -1.078873, 0.381903, 0.324071 },   { -0.938392, 1.051765, 0.324071 },
    { 0.000000, 0.722260, 0.032758 },    { -0.000000, -0.709850, 0.109850 },
    { 1.750623, -1.379782, 0.578682 },   { 1.935871, -0.470816, 0.578682 },
    { 1.906424, 0.603645, 0.578682 },    { 1.594696, 1.602469, 0.578682 },
    { -1.750623, -1.379782, 0.578682 },  { -1.935871, -0.470816, 0.578682 },
    { -1.906424, 0.603645, 0.578682 },   { -1.594696, 1.602469, 0.578682 },
    { 2.765063, -1.965469, -0.720684 },  { 3.089452, -0.674223, -0.720687 },
    { 3.037887, 0.906820, -0.720687 },   { 2.492023, 2.355415, -0.720687 },
    { -2.765065, -1.965471, -0.720687 }, { -3.089452, -0.674223, -0.720687 },
    { -3.037887, 0.906820, -0.720687 },  { -2.492023, 2.355415, -0.720687 },
    { 2.387326, -1.747383, 0.021827 },   { 2.659903, -0.598482, 0.021827 },
    { 2.616574, 0.793929, 0.021827 },    { 2.157893, 2.075047, 0.021827 },
    { -2.387326, -1.747383, 0.021827 },  { -2.659903, -0.598482, 0.021827 },
    { -2.616574, 0.793929, 0.021827 },   { -2.157893, 2.075047, 0.021827 },
    { 0.503923, -0.660000, -0.000000 },  { 0.518177, -0.220838, 0.000000 },
    { 0.515911, 0.231058, 0.000000 },    { 0.491925, 0.677134, -0.000000 },
    { -0.503923, -0.660000, 0.000000 },  { -0.518177, -0.220838, 0.000000 },
    { -0.515911, 0.231058, 0.000000 },   { -0.491925, 0.677134, 0.000000 }
  };

  mSubstrate = new NEAT::Substrate(inputs, hidden, outputs);

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
