#pragma once

#include "Experiment.hpp"

/**
 * @brief
 *   This experiment defines a simple experiments for creating
 *   robots that stand entirely still.
 */
class StandingSimpleInputs : public Experiment {
public:
  StandingSimpleInputs();
  ~StandingSimpleInputs();

  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
