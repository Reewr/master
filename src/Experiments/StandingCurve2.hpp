#pragma once

#include "Experiment.hpp"

class StandingCurve2 : public Experiment {
public:
  StandingCurve2();
  ~StandingCurve2();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
