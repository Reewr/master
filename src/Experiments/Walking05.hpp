#pragma once

#include "Experiment.hpp"

class Walking05 : public Experiment {
public:
  Walking05();
  ~Walking05();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
