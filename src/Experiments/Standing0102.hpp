#pragma once

#include "Experiment.hpp"

class Standing0102 : public Experiment {
public:
  Standing0102();
  ~Standing0102();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
