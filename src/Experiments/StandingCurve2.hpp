#pragma once

#include "Experiment.hpp"

/**
 * @brief
 *   This experiment defines an experiment using HyperNEAT
 *   to make the robot walk using a 2D substrate that takes
 *   an input for each active joint and gets its rotations as a
 *   normalized value between -1 and 1 where -1 indicates -1*PI.
 *
 *   In addition, it also takes an input of whether a leg is toching
 *   the ground or not.
 */
class StandingCurve2 : public Experiment {
public:
  StandingCurve2();
  ~StandingCurve2();

  float mergeFitnessValues(const mmm::vec<9>& fitness) const;
  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
