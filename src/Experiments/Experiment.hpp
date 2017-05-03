#pragma once

#include <vector>

#include "../Learning/Fitness.hpp"
#include "../Log.hpp"

#include <Genome.h>
#include <Population.h>

class Substrate;
struct Phenotype;

struct ExperimentParameters {
  // Describes the amount of activations that the network
  // should perform.
  int numActivates = 4;

  // Performs the static deltaTime that should be used.
  float deltaTime  = 1.0f / 60.f;

  // Sets the experiment duration
  unsigned int experimentDuration = 10;

  // Sets the preperation duration
  unsigned int preperationDuration = 1;
};

/**
 * @brief
 *   This function serves as a template for implementations.
 *
 *   Inheriting from this class allows you to create an experiement
 *   that can set entirely different parameters and substrate. It can
 *   also set what fitness functions to use, what input to add and lastly
 *   how to use the output.
 */
class Experiment : public Logging::Log {
public:

  virtual ~Experiment();

  // Returns the set substrate for the Experiment
  Substrate* substrate() const;

  // Returns the population settings for the Experiment
  NEAT::Population* population() const;

  void setSubstrate(Substrate* sub);
  void setPopulation(NEAT::Population* pop);

  // Returns the parameters for the Experiments
  const NEAT::Parameters& neatParameters() const;

  // Returns the parameters that are not specific to MultiNEAT
  const ExperimentParameters& parameters() const;

  // Returns the name of the experiment
  const std::string& name() const;

  // Returns the number of expected inputs
  unsigned int numInputs() const;

  // Returns the fitness functions
  const std::vector<Fitness>& fitnessFunctions() const;

  // Tells the experiment to use the outputs from the network
  virtual void outputs(Phenotype& p, const std::vector<double>& outputs) const = 0;

  // Tells the experiment to retrieve inputs
  virtual std::vector<double> inputs(const Phenotype& p) const = 0;

protected:
  Experiment(const std::string& name);

  std::string          mName;
  ExperimentParameters mParameters;
  NEAT::Population*    mPopulation       = nullptr;
  Substrate*           mSubstrate        = nullptr;
  std::vector<Fitness> mFitnessFunctions = {};
};
