#pragma once

#include <map>
#include <vector>

struct Phenotype;

/**
 * @brief
 *   This class is used to store statistics of
 *   the simulations.
 *
 *   It should be called after each epoch, but before each
 *   recreation of Phenotypes.
 */
class Statistics {
public:
  Statistics();
  Statistics(const std::string& filename);

  void addEntry(const std::vector<Phenotype>& phenotypes,
                unsigned int                  generation);

  void save(const std::string& filename) const;


private:
  // Entries in the statistics
  struct Entry {
    float              finalizedFitness;
    std::vector<float> individualFitness;
    unsigned int       speciesIndex;
    unsigned int       individualIndex;
    bool               bestOfSpecies;
    bool               bestOfGeneration;
  };

  std::map<unsigned int, std::vector<Entry>> mEntries;
};
