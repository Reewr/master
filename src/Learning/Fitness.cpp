#include "Fitness.hpp"

Fitness::Fitness(const std::string& name,
                 const std::string& longDesc,
                 Calculation        calc,
                 Calculation        finalize)
    : mName(name)
    , mDescription(longDesc)
    , mContinuousCalculation(calc)
    , mFinalize(finalize) {}

/**
 * @brief
 *
 *  Runs the function associated with this fitness value,
 *  returns the new fitness value
 *
 * @param s
 * @param fitness
 * @param deltaValue
 *
 * @return
 */
float Fitness::runCalculation(const std::map<std::string, Spider::Part>& s,
                              float               fitness,
                              float               deltaValue) const {
  if (mContinuousCalculation)
    return mContinuousCalculation(s, fitness, deltaValue);
  return fitness;
}

/**
 * @brief
 *   Performs one last calculation of the fitness, running
 *   the finalize function if it exists.
 *
 *   If no function has been defined for calculating the final
 *   value, the fitness given is returned.
 *
 * @param s
 * @param fitness
 * @param deltaValue
 *
 * @return
 */
float Fitness::runFinalize(const std::map<std::string, Spider::Part>& s,
                           float               fitness,
                           float               deltaValue) const {
  if (mFinalize)
    return mFinalize(s, fitness, deltaValue);
  return fitness;
}

/**
 * @brief
 *   Returns the description
 *
 * @return
 */
const std::string& Fitness::desc() const {
  return mDescription;
}

/**
 * @brief
 *   Returns the description
 *
 * @return
 */
const std::string& Fitness::name() const {
  return mName;
}

/**
 * @brief
 *   Makes this class printable
 *
 * @param os
 * @param fitness
 *
 * @return
 */
std::ostream& operator<<(std::ostream& os, const Fitness& fitness) {
  os << fitness.desc();
  return os;
}
