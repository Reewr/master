#pragma once

#include "Experiment.hpp"

class WalkingRT : public Experiment {
public:
  WalkingRT();
  ~WalkingRT();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
  void postUpdate(const Phenotype& p) const;
};
