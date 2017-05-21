#pragma once

#include "Experiment.hpp"

class Walking0102 : public Experiment {
public:
  Walking0102();
  ~Walking0102();

  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
