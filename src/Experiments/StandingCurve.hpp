#pragma once

#include "Experiment.hpp"

class StandingCurve : public Experiment {
public:
  StandingCurve();
  ~StandingCurve();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
