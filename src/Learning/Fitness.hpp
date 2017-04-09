#pragma once

#include "../3D/Spider.hpp"
#include <functional>

struct Phenotype;

/**
 * @brief
 *   In order to keep a structure on the fitness calculations, we've
 *   added a class that should help document it.
 *
 *   The instances of these classes are meant to be static so that they
 *   can be used by any spider instance. The reason for that is that they
 *   do not hold any state and this reduces the memory required.
 */
class Fitness {
public:
  typedef std::function<float(const Phenotype&, float, float)> Calculation;

  Fitness(const std::string& name,
          const std::string& longDesc,
          Calculation        calc,
          Calculation        finalize = nullptr);

  // Runs a calculation on the spider part and the fitness, performing the
  // calculation
  // function.
  float runCalculation(const Phenotype& phenotype,
                       float            fitness,
                       float            deltaValue) const;

  // Prior to sending the fitness to HyperNeat, one additional calculation may
  // be done to finalize values.
  float runFinalize(const Phenotype& phenotype,
                    float            fitness,
                    float            deltaValue) const;

  // Returns the description
  const std::string& desc() const;
  const std::string& name() const;

  friend std::ostream& operator<<(std::ostream& os, const Fitness& fitness);

private:
  std::string mName;
  std::string mDescription;

  Calculation mContinuousCalculation;
  Calculation mFinalize;
};
