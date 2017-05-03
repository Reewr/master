#pragma once

#include "Experiment.hpp"

/**
 * @brief
 *   This experiment defines a simple experiments for creating
 *   robots that stand entirely still. It only takes 8 inputs, one
 *   for each leg of whether or not it is touching the ground or not.
 *
 *   No result after 2759 generations. It seems like it cant handle standing with
 *   very inputs. This makes sense though.
 */
class StandingSimpleInputs : public Experiment {
public:
  StandingSimpleInputs();
  ~StandingSimpleInputs();

  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
