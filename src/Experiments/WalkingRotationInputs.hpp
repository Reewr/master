#pragma once

#include "Experiment.hpp"

class WalkingRotationInputs : public Experiment {
public:
  WalkingRotationInputs();
  ~WalkingRotationInputs();

  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
