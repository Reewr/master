#pragma once

#include "Experiment.hpp"

class WalkingRotationNoKill : public Experiment {
public:
  WalkingRotationNoKill();
  ~WalkingRotationNoKill();

  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
