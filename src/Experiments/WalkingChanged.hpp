#pragma once

#include "Experiment.hpp"

class WalkingChanged : public Experiment {
public:
  WalkingChanged();
  ~WalkingChanged();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
