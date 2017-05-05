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
