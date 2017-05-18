#include "Experiment.hpp"

#include <Population.h>
#include "../Learning/Substrate.hpp"

Experiment::Experiment(const std::string& name) : Logging::Log(name), mName(name) {}
Experiment::~Experiment() {}

/**
 * @brief
 *   Returns the substrate of the experiment.
 *
 *   If the experiment hasnt defined a substrate, it may
 *   be nullptr, in which case the experiment should fail.
 *
 * @return
 */
Substrate* Experiment::substrate() const {
  return mSubstrate;
}

/**
 * @brief
 *   Returns the population of the experiment.
 *
 *   If the experiment hasnt defined a population, it may
 *   be nullptr, in which case the experiment should fail.
 *
 * @return
 */
NEAT::Population* Experiment::population() const {
  return mPopulation;
}

/**
 * @brief
 *   Sets the experiments substrates.
 *
 *   If the experiment already has a substrate,
 *   it will delete it.
 *
 * @return
 */
void Experiment::setSubstrate(Substrate* sub) {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  mSubstrate = sub;
}

/**
 * @brief
 *   Sets the experiments population.
 *
 *   If the experiment already has a population,
 *   it will delete it.
 *
 * @return
 */
void Experiment::setPopulation(NEAT::Population* pop) {
  if (mPopulation != nullptr)
    delete mPopulation;

  mPopulation = pop;
}

/**
 * @brief
 *   Returns a reference to the NEAT::Parameters
 *
 * @return
 */
const NEAT::Parameters& Experiment::neatParameters() const {
  return mPopulation->m_Parameters;
}

/**
 * @brief
 *   Returns a reference to the ExperimentParameters
 *
 * @return
 */
const ExperimentParameters& Experiment::parameters() const {
  return mParameters;
}

/**
 * @brief
 *   Returns a reference to the name of the experiment
 *
 * @return
 */
const std::string& Experiment::name() const {
  return mName;
}

/**
 * @brief
 *   Returns the number of expected inputs
 *
 * @return
 */
unsigned int Experiment::numInputs() const {
  return mSubstrate->m_input_coords.size();
}

/**
 * @brief
 *   Returns a reference to the list of fitness functions
 *
 * @return
 */
const std::vector<Fitness>& Experiment::fitnessFunctions() const {
  return mFitnessFunctions;
}

/**
 * @brief
 *   Returns the default parameters for this project.
 *   These are not nessassarily the default parameters for MultiNEAT.
 *
 * @return
 */
NEAT::Parameters Experiment::getDefaultParameters() const {

  NEAT::Parameters params;
  // Below follows all the parameters. The first comment is described
  // by the author of the library and "US" is a comment from our side to
  // explain more on the subject
  //
  // If there is a commented out value behind the value, then that represents
  // the default value for that parameter
  //
  // Many of these values are the default values of MultiNEAT, not used or have
  // no effect, but are added here for readability

  ////////////////////
  // Basic parameters
  ////////////////////

  // Size of population
  params.PopulationSize = 150;

  // If true, this enables dynamic compatibility thresholding
  // It will keep the number of species between MinSpecies and MaxSpecies
  params.DynamicCompatibility = true;

  // Minimum number of species
  params.MinSpecies = 10;

  // Maximum number of species
  params.MaxSpecies = 15;

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
  params.SpeciesMaxStagnation = 15;

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
  params.SurvivalRate = 0.20;

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
  params.MultipointCrossoverRate = 0.65;

  // Performing roulette wheel selection or not?
  params.RouletteWheelSelection = false;

  // For tournament selection
  // Does not matter unless RouletteWheelSelection is true
  params.TournamentSize = 4;

  // Fraction of individuals to be copied unchanged
  params.EliteFraction = 0.1;

  ///////////////////////////////////
  // Structural Mutation parameters
  ///////////////////////////////////

  // Probability for a baby to be mutated with the Add-Neuron mutation.
  params.MutateAddNeuronProb = 0.04;

  // Allow splitting of any recurrent links
  params.SplitRecurrent = true;

  // Allow splitting of looped recurrent links
  params.SplitLoopedRecurrent = true;

  // Probability for a baby to be mutated with the Add-Link mutation
  params.MutateAddLinkProb = 0.4;

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
  params.RecurrentProb = 0.5;

  // Probability that a recurrent link mutation will be looped
  params.RecurrentLoopProb = 0.25;

  ///////////////////////////////////
  // Parameter Mutation parameters
  ///////////////////////////////////

  // Probability for a baby's weights to be mutated
  params.MutateWeightsProb = 0.8;

  // Probability for a severe (shaking) weight mutation
  params.MutateWeightsSevereProb = 0.5;

  // Probability for a particular gene's weight to be mutated. 1.0 = 100%
  params.WeightMutationRate = 0.96;

  // Maximum perturbation for a weight mutation
  params.WeightMutationMaxPower = 0.5;

  // Maximum magnitude of a replaced weight
  params.WeightReplacementMaxPower = 1.0;

  // Maximum absolute magnitude of a weight
  // US: This only affects the weight in the CPPN and not the
  //     weights in the connection generated by the CPPN for the
  //     network we use.
  params.MaxWeight = 3.0;

  // Probability for a baby's A activation function parameters to be perturbed
  params.MutateActivationAProb = 0.1;

  // Probability for a baby's B activation function parameters to be perturbed
  params.MutateActivationBProb = 0.0;

  // Maximum magnitude for the A parameter perturbation
  params.ActivationAMutationMaxPower = 0.2;

  // Maximum magnitude for the B parameter perturbation
  params.ActivationBMutationMaxPower = 0.0;

  // Activation parameter A min/max
  params.MinActivationA = 1.0;
  params.MaxActivationA = 6.0;

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
  params.MinNeuronBias = -params.MaxWeight;
  params.MaxNeuronBias = params.MaxWeight;

  // Probability for a baby that an activation function type will be changed for a single neuron
  // considered a structural mutation because of the large impact on fitness
  params.MutateNeuronActivationTypeProb = 0.15;

  // Probabilities for a particular activation function appearance
  params.ActivationFunction_SignedSigmoid_Prob = 1.0;
  params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
  params.ActivationFunction_Tanh_Prob = 0.0;
  params.ActivationFunction_TanhCubic_Prob = 0.0;
  params.ActivationFunction_SignedStep_Prob = 0.0;
  params.ActivationFunction_UnsignedStep_Prob = 0.0;
  params.ActivationFunction_SignedGauss_Prob = 1.0;
  params.ActivationFunction_UnsignedGauss_Prob = 0.0;
  params.ActivationFunction_Abs_Prob = 0.0;
  params.ActivationFunction_SignedSine_Prob = 1.0;
  params.ActivationFunction_UnsignedSine_Prob = 0.0;
  params.ActivationFunction_Linear_Prob = 1.0;
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
  params.WeightDiffCoeff = 1.0;

  // Node-specific activation parameter A difference importance
  params.ActivationADiffCoeff = 0.0;

  // Node-specific activation parameter B difference importance
  params.ActivationBDiffCoeff = 0.0;

  // Average time constant difference importance
  params.TimeConstantDiffCoeff = 0.0;

  // Average bias difference importance
  params.BiasDiffCoeff = 0.0;

  // Activation function type difference importance
  params.ActivationFunctionDiffCoeff = 1.0;

  // Compatibility treshold
  // If dynamiccompathreshold this will change
  params.CompatTreshold = 3.0;

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

  return params;
}

/**
 * @brief
 *   Creates the default substrate used in this project.
 *
 * @return
 */
Substrate* Experiment::createDefaultSubstrate() const {
  std::vector<std::vector<double>> inputs{
   { -1.00,  1.00, -1.00 }, // MiscL1
   { -0.71,  1.00, -1.00 }, // MiscL2
   { -0.42,  1.00, -1.00 }, // MiscL3
   { -0.14,  1.00, -1.00 }, // MiscL4
   {  1.00,  1.00, -1.00 }, // MiscR1
   {  0.71,  1.00, -1.00 }, // MiscR2
   {  0.42,  1.00, -1.00 }, // MiscR3
   {  0.14,  1.00, -1.00 }, // MiscR4

   { -1.00,  0.66, -1.00 }, // TouchL1
   { -0.71,  0.66, -1.00 }, // TouchL2
   { -0.42,  0.66, -1.00 }, // TouchL3
   { -0.14,  0.66, -1.00 }, // TouchL4
   {  1.00,  0.66, -1.00 }, // TouchR1
   {  0.71,  0.66, -1.00 }, // TouchR2
   {  0.42,  0.66, -1.00 }, // TouchR3
   {  0.14,  0.66, -1.00 }, // TouchR4

   { -1.00, -0.66, -1.00 }, // FemurL1
   { -0.71, -0.66, -1.00 }, // FemurL2
   { -0.42, -0.66, -1.00 }, // FemurL3
   { -0.14, -0.66, -1.00 }, // FemurL4
   {  1.00, -0.66, -1.00 }, // FemurR1
   {  0.71, -0.66, -1.00 }, // FemurR2
   {  0.42, -0.66, -1.00 }, // FemurR3
   {  0.14, -0.66, -1.00 }, // FemurR4

   { -1.00, -0.33, -1.00 }, // PatellaL1
   { -0.71, -0.33, -1.00 }, // PatellaL2
   { -0.42, -0.33, -1.00 }, // PatellaL3
   { -0.14, -0.33, -1.00 }, // PatellaL4
   {  1.00, -0.33, -1.00 }, // PatellaR1
   {  0.71, -0.33, -1.00 }, // PatellaR2
   {  0.42, -0.33, -1.00 }, // PatellaR3
   {  0.14, -0.33, -1.00 }, // PatellaR4

   { -1.00,  0.33, -1.00 }, // TarsusL1
   { -0.71,  0.33, -1.00 }, // TarsusL2
   { -0.42,  0.33, -1.00 }, // TarsusL3
   { -0.14,  0.33, -1.00 }, // TarsusL4
   {  1.00,  0.33, -1.00 }, // TarsusR1
   {  0.71,  0.33, -1.00 }, // TarsusR2
   {  0.42,  0.33, -1.00 }, // TarsusR3
   {  0.14,  0.33, -1.00 }, // TarsusR4

   { -1.00,  0.00, -1.00 }, // TibiaL1
   { -0.71,  0.00, -1.00 }, // TibiaL2
   { -0.42,  0.00, -1.00 }, // TibiaL3
   { -0.14,  0.00, -1.00 }, // TibiaL4
   {  1.00,  0.00, -1.00 }, // TibiaR1
   {  0.71,  0.00, -1.00 }, // TibiaR2
   {  0.42,  0.00, -1.00 }, // TibiaR3
   {  0.14,  0.00, -1.00 }, // TibiaR4

   { -1.00, -1.00, -1.00 }, // TrochanterL1
   { -0.71, -1.00, -1.00 }, // TrochanterL2
   { -0.42, -1.00, -1.00 }, // TrochanterL3
   { -0.14, -1.00, -1.00 }, // TrochanterL4
   {  1.00, -1.00, -1.00 }, // TrochanterR1
   {  0.71, -1.00, -1.00 }, // TrochanterR2
   {  0.42, -1.00, -1.00 }, // TrochanterR3
   {  0.14, -1.00, -1.00 }  // TrochanterR4
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{};

  // clang-format on
  // clone the input neuron positions to hidden, but at different height
  for (auto& x : inputs) {
    outputs.push_back(std::vector<double>{x[0], x[1], 1.0});
    hidden.push_back(std::vector<double>{x[0], x[1], 0.0});
  }

  Substrate* substrate = new Substrate(inputs, hidden, outputs);

  substrate->m_allow_input_hidden_links  = true;
  substrate->m_allow_input_output_links  = true;
  substrate->m_allow_hidden_output_links = true;
  substrate->m_allow_hidden_hidden_links = true;
  substrate->m_allow_output_hidden_links = true;
  substrate->m_allow_output_output_links = true;
  substrate->m_allow_looped_hidden_links = true;
  substrate->m_allow_looped_output_links = true;

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
  substrate->m_hidden_nodes_activation = NEAT::ActivationFunction::SIGNED_SIGMOID;
  substrate->m_output_nodes_activation = NEAT::ActivationFunction::SIGNED_SIGMOID;
  substrate->m_max_weight_and_bias = 2.0;

  return substrate;
}

/**
 * @brief
 *   Returns the total duration for the experiment, including
 *   the experiment duration and preperation duration
 *
 * @return
 */
float Experiment::totalDuration() const {
  return mParameters.experimentDuration + mParameters.preperationDuration;
}

/**
 * @brief
 *   Allows you to initialize a phenotype. Prior to this function
 *   all the phenotypes are completely reset.
 *
 *   This function allows you to change some of its variables, such
 *   as starting fitness values.
 *
 *   Often this can be used to set all the fitness values to start
 *   as 1 instead of 0, so you can perform product on the fitness
 *   values
 *
 *   Default behaviour is to do nothing at all
 *
 * @param Phenotype
 */
void Experiment::initPhenotype(Phenotype&) const {}

/**
 * @brief
 *   Allows you to perform some last changes or calculations before the
 *   generation is ended.
 *
 * @param Phenotype
 */
void Experiment::postUpdate(const Phenotype&) const {}

/**
 * @brief
 *   Allows you to customize the way that the fitness values are merged
 *
 *   Default behaviour is to sum all fitness values together
 *
 * @param fitnesses
 *
 * @return
 */
float Experiment::mergeFitnessValues(const mmm::vec<9>& fitnesses) const {
  return mmm::sum(fitnesses);
}
