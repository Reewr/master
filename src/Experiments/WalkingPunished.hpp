#pragma once

#include "Experiment.hpp"

class WalkingPunished : public Experiment {
public:
  WalkingPunished();
  ~WalkingPunished();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
