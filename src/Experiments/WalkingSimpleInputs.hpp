#pragma once

#include "Experiment.hpp"

/**
 * @brief
 *   This experiment defines an experiment using HyperNEAT
 *   to make the robot walk using a 2D substrate that only
 *   takes in 8 inputs, indicating whether or not
 *   the legs are touching the ground.
 *
 *   No result after 2759 generations. It seems like it cant handle standing with
 *   very inputs. This makes sense though.
 */
class WalkingSimpleInputs : public Experiment {
public:
  WalkingSimpleInputs();
  ~WalkingSimpleInputs();

  void outputs(Phenotype& p, const std::vector<double>& outputs) const;
  std::vector<double> inputs(const Phenotype& p) const;
};
