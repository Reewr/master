#include "StandingSimpleInputs.hpp"

#include "../Learning/Phenotype.hpp"
#include "../Learning/Substrate.hpp"

#include "ExperimentUtil.hpp"

#include <btBulletDynamicsCommon.h>

const float PI = mmm::constants<float>::pi;

StandingSimpleInputs::StandingSimpleInputs() : Experiment("StandingSimpleInputs") {

  mFitnessFunctions =
  { Fitness("No movements",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float current, float dt) -> float {
              if (current == 0)
                current = 1.0;

              const auto&        parts = p.spider->parts();
              const btRigidBody* sternum =
                parts.at("Sternum").part->rigidBody();
              const btVector3& massPos = sternum->getCenterOfMassPosition();

              return ExpUtil::score(dt, massPos.z()) *
                     ExpUtil::score(dt, massPos.x()) * current;
            }),
    Fitness("Time Alive",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float current, float dt) -> float {
              if (p.hasBeenKilled())
                return current;
              return current + dt;
            },
            [](const Phenotype& p, float current, float duration) -> float {
              return current * 2 / duration;
            }),
    Fitness("Resting angles",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float current, float dt) -> float {
              if (current == 0)
                current = 1.0;

              const auto& parts = p.spider->parts();

              for(auto& part : parts) {
                if (!part.second.active || part.second.hinge == nullptr)
                  continue;

                float normalized = ExpUtil::normalizeAngle(part.second.hinge->getHingeAngle(),
                                                           -PI,
                                                           PI,
                                                           part.second.restAngle);

                current *= ExpUtil::score(dt, normalized);
              }

              return current;
            }),
    Fitness("Colliding",
            "If the spider falls, stop the simulation",
            [](const Phenotype& phenotype, float current, float) -> float {

              if (phenotype.collidesWithTerrain("Abdomin") ||
                  phenotype.collidesWithTerrain("FemurR1") ||
                  phenotype.collidesWithTerrain("FemurR2") ||
                  phenotype.collidesWithTerrain("FemurR3") ||
                  phenotype.collidesWithTerrain("FemurR4") ||
                  phenotype.collidesWithTerrain("FemurL1") ||
                  phenotype.collidesWithTerrain("FemurL2") ||
                  phenotype.collidesWithTerrain("FemurL3") ||
                  phenotype.collidesWithTerrain("FemurL4") ||
                  phenotype.collidesWithTerrain("TrochanterR1") ||
                  phenotype.collidesWithTerrain("TrochanterR2") ||
                  phenotype.collidesWithTerrain("TrochanterR3") ||
                  phenotype.collidesWithTerrain("TrochanterR4") ||
                  phenotype.collidesWithTerrain("TrochanterL1") ||
                  phenotype.collidesWithTerrain("TrochanterL2") ||
                  phenotype.collidesWithTerrain("TrochanterL3") ||
                  phenotype.collidesWithTerrain("TrochanterL4") ||
                  phenotype.collidesWithTerrain("TibiaR1") ||
                  phenotype.collidesWithTerrain("TibiaR2") ||
                  phenotype.collidesWithTerrain("TibiaR3") ||
                  phenotype.collidesWithTerrain("TibiaR4") ||
                  phenotype.collidesWithTerrain("TibiaL1") ||
                  phenotype.collidesWithTerrain("TibiaL2") ||
                  phenotype.collidesWithTerrain("TibiaL3") ||
                  phenotype.collidesWithTerrain("TibiaL4") ||
                  phenotype.collidesWithTerrain("PatellaR1") ||
                  phenotype.collidesWithTerrain("PatellaR2") ||
                  phenotype.collidesWithTerrain("PatellaR3") ||
                  phenotype.collidesWithTerrain("PatellaR4") ||
                  phenotype.collidesWithTerrain("PatellaL1") ||
                  phenotype.collidesWithTerrain("PatellaL2") ||
                  phenotype.collidesWithTerrain("PatellaL3") ||
                  phenotype.collidesWithTerrain("PatellaL4") ||
                  phenotype.collidesWithTerrain("Abdomin") ||
                  phenotype.collidesWithTerrain("Hip") ||
                  phenotype.collidesWithTerrain("Sternum") ||
                  phenotype.collidesWithTerrain("Neck") ||
                  phenotype.collidesWithTerrain("Eye")) {
                phenotype.kill();
              }

              return current;
            }),
  };

  std::vector<std::vector<double>> inputs{
    {  0.1, 0.0 }, // TipL1
    {  0.2, 0.0 }, // TipL2
    {  0.3, 0.0 }, // TipL3
    {  0.4, 0.0 }, // TipL4

    {  0.6, 0.0 }, // TipR1
    {  0.7, 0.0 }, // TipR2
    {  0.8, 0.0 }, // TipR3
    {  0.9, 0.0 }, // TipR4
  };
  std::vector<std::vector<double>> hidden{};
  std::vector<std::vector<double>> outputs{
    {  0.12,  1.0 }, // FemurL1
    {  0.22,  1.0 }, // FemurL2
    {  0.32,  1.0 }, // FemurL3
    {  0.42,  1.0 }, // FemurL4

    { 0.58,  1.0 }, // FemurR1
    { 0.68,  1.0 }, // FemurR2
    { 0.78,  1.0 }, // FemurR3
    { 0.88,  1.0 }, // FemurR4

    { 0.08, 1.0 }, // PatellaL1
    { 0.18, 1.0 }, // PatellaL2
    { 0.28, 1.0 }, // PatellaL3
    { 0.38, 1.0 }, // PatellaL4

    { 0.62, 1.0 }, // PatellaR1
    { 0.72, 1.0 }, // PatellaR2
    { 0.82, 1.0 }, // PatellaR3
    { 0.92, 1.0 }, // PatellaR4

    { 0.04,  1.0 }, // TibiaL1
    { 0.14,  1.0 }, // TibiaL2
    { 0.24,  1.0 }, // TibiaL3
    { 0.34,  1.0 }, // TibiaL4

    { 0.66, 1.0 }, // TibiaR1
    { 0.76, 1.0 }, // TibiaR2
    { 0.86, 1.0 }, // TibiaR3
    { 0.96, 1.0 }, // TibiaR4

    { 0.16, 1.0 }, // TrochanterL1
    { 0.26, 1.0 }, // TrochanterL2
    { 0.36, 1.0 }, // TrochanterL3
    { 0.46, 1.0 }, // TrochanterL4

    { 0.54,  1.0 }, // TrochanterR1
    { 0.64,  1.0 }, // TrochanterR2
    { 0.74,  1.0 }, // TrochanterR3
    { 0.84,  1.0 }  // TrochanterR4
  };
  // clang-format on


  // clone the input neuron positions to hidden, but at different height
  for (auto& x : outputs)
    hidden.push_back(std::vector<double>{x[0], 0.5});

  mSubstrate = new Substrate(inputs, hidden, outputs);

  // These variables are only used in HyperNEAT variations and not
  // in ESHyperNEAT.
  mSubstrate->m_allow_input_hidden_links  = true;
  mSubstrate->m_allow_input_output_links  = true;
  mSubstrate->m_allow_hidden_output_links = true;
  mSubstrate->m_allow_hidden_hidden_links = true;
  mSubstrate->m_allow_output_hidden_links = true;
  mSubstrate->m_allow_output_output_links = true;
  mSubstrate->m_allow_looped_hidden_links = true;
  mSubstrate->m_allow_looped_output_links = true;

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
  mSubstrate->m_hidden_nodes_activation = NEAT::ActivationFunction::TANH;
  mSubstrate->m_output_nodes_activation = NEAT::ActivationFunction::TANH;

  // This is only available in HyperNEAT and not ESHyperNEAT
  mSubstrate->m_leaky = true;
  mSubstrate->m_min_time_const = 0.1;
  mSubstrate->m_max_time_const = 2.0;
  mSubstrate->m_query_weights_only = false;

  // When a new connection, it will not be added if the weight*maxWeightAndBias
  // is less than 0.2
  mSubstrate->m_max_weight_and_bias = 4.0;

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
  params.MutateAddNeuronProb = 0.2;

  // Allow splitting of any recurrent links
  params.SplitRecurrent = true;

  // Allow splitting of looped recurrent links
  params.SplitLoopedRecurrent = true;

  // Probability for a baby to be mutated with the Add-Link mutation
  params.MutateAddLinkProb = 0.2;

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
  params.MaxWeight = 4.0;

  // Probability for a baby's A activation function parameters to be perturbed
  params.MutateActivationAProb = 0.1;

  // Probability for a baby's B activation function parameters to be perturbed
  params.MutateActivationBProb = 0.1;

  // Maximum magnitude for the A parameter perturbation
  params.ActivationAMutationMaxPower = 0.0;

  // Maximum magnitude for the B parameter perturbation
  params.ActivationBMutationMaxPower = 0.0;

  // Activation parameter A min/max
  params.MinActivationA = 0.0;
  params.MaxActivationA = 2.0;

  // Activation parameter B min/max
  params.MinActivationB = 0.0;
  params.MaxActivationB = 2.0;

  // Maximum magnitude for time costants perturbation
  params.TimeConstantMutationMaxPower = 0.1;

  // Maximum magnitude for biases perturbation
  params.BiasMutationMaxPower = params.WeightMutationMaxPower;

  // Probability for a baby's neuron time constant values to be mutated
  params.MutateNeuronTimeConstantsProb = 0.0;

  // Probability for a baby's neuron bias values to be mutated
  params.MutateNeuronBiasesProb = 0.0;

  // Time constant range
  params.MinNeuronTimeConstant = 0.1;
  params.MaxNeuronTimeConstant = 2.0;

  // Bias range
  params.MinNeuronBias = -4.0;
  params.MaxNeuronBias = 4.0;

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
  params.ActivationFunction_Linear_Prob = 1.0;
  params.ActivationFunction_Relu_Prob = 1.0;
  params.ActivationFunction_Softplus_Prob = 1.0;

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
                      NEAT::ActivationFunction::TANH,
                      NEAT::ActivationFunction::TANH,
                      0,
                      params);

  mPopulation = new NEAT::Population(genome, params, true, 1.0, time(0));
  mParameters.numActivates = 8;
}

StandingSimpleInputs::~StandingSimpleInputs() {
  delete mPopulation;
  delete mSubstrate;
}

void StandingSimpleInputs::outputs(Phenotype&                 p,
                                  const std::vector<double>& outputs) const {
  size_t index = 0;
  for(auto& part : p.spider->parts()) {
    if (part.second.hinge == nullptr)
      continue;

    float currentAngle = part.second.hinge->getHingeAngle();
    float velocity;

    if (part.second.active) {
      float output = outputs[index];
      currentAngle = ExpUtil::normalizeAngle(currentAngle, -PI, PI, 0);
      velocity     = output - currentAngle;
      index++;
    } else {
      velocity = mmm::clamp(part.second.restAngle - currentAngle, -0.3f, 0.3f) * 16.0f;
    }

    part.second.hinge->enableAngularMotor(true, velocity, 16.f);
  }
}

std::vector<double> StandingSimpleInputs::inputs(const Phenotype& p) const {
  std::vector<double> inputs = {
    p.collidesWithTerrain("TarsusL1") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusL2") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusL3") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusL4") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusR1") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusR2") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusR3") ? 1.0 : 0.0,
    p.collidesWithTerrain("TarsusR4") ? 1.0 : 0.0,
  };

  return inputs;
}
