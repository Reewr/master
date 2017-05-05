#include "Experiment.hpp"

#include <Population.h>
#include "../Learning/Substrate.hpp"

Experiment::Experiment(const std::string& name) : Logging::Log(name), mName(name) {}
Experiment::~Experiment() {}

Substrate* Experiment::substrate() const {
  return mSubstrate;
}

NEAT::Population* Experiment::population() const {
  return mPopulation;
}

void Experiment::setSubstrate(Substrate* sub) {
  if (mSubstrate != nullptr)
    delete mSubstrate;

  mSubstrate = sub;
}

void Experiment::setPopulation(NEAT::Population* pop) {
  if (mPopulation != nullptr)
    delete mPopulation;

  mPopulation = pop;
}

const NEAT::Parameters& Experiment::neatParameters() const {
  return mPopulation->m_Parameters;
}

const ExperimentParameters& Experiment::parameters() const {
  return mParameters;
}

const std::string& Experiment::name() const {
  return mName;
}

unsigned int Experiment::numInputs() const {
  return mSubstrate->m_input_coords.size();
}

const std::vector<Fitness>& Experiment::fitnessFunctions() const {
  return mFitnessFunctions;
}

float Experiment::mergeFitnessValues(const mmm::vec<9>& fitnesses) const {
  return mmm::sum(fitnesses);
}
