#include "Statistics.hpp"

#include <fstream>

#include "Phenotype.hpp"

template<size_t n>
std::vector<float> vecNtoList(const mmm::vec<n>& t) {
  std::vector<float> elements;

  for(size_t i = 0; i < n; i++)
    elements.push_back(t[i]);

  return elements;
}

Statistics::Statistics() {}

/**
 * @brief
 *   Adds the generation to the statistics. Uses all the phenotypes
 *   to find the best of each species and the best total
 *
 * @param phenotypes
 * @param generation
 */
void Statistics::addEntry(const std::vector<Phenotype>& phenotypes,
                          unsigned int generation) {
  mEntries[generation] = {};
  mEntries[generation].reserve(phenotypes.size());

  std::map<unsigned int, const Phenotype*> leaders;
  const Phenotype* absoluteBest = nullptr;

  // Find the best phenotype and the best of each species
  for (const Phenotype& phenotype : phenotypes) {
    if (absoluteBest == nullptr)
      absoluteBest = &phenotype;
    else if (absoluteBest->finalizedFitness < phenotype.finalizedFitness)
      absoluteBest = &phenotype;

    if (leaders.count(phenotype.speciesIndex) &&
        leaders[phenotype.speciesIndex]->finalizedFitness < phenotype.finalizedFitness)
      leaders[phenotype.speciesIndex] = &phenotype;
    else if (leaders.count(phenotype.speciesIndex) == 0)
      leaders[phenotype.speciesIndex] = &phenotype;
  }

  // Make and add the entries
  for (const Phenotype& phenotype : phenotypes) {
    Entry e;

    e.speciesIndex = phenotype.speciesIndex;
    e.individualIndex = phenotype.individualIndex;
    e.finalizedFitness = phenotype.finalizedFitness;
    e.individualFitness = vecNtoList(phenotype.fitness);

    if (leaders[phenotype.speciesIndex]->individualIndex == phenotype.individualIndex)
      e.bestOfSpecies = true;
    if (absoluteBest->speciesIndex == phenotype.speciesIndex &&
        absoluteBest->individualIndex == phenotype.individualIndex)
      e.bestOfGeneration = true;

    mEntries[generation].push_back(e);
  }
}

/**
 * @brief
 *   Saves the statistics as a CSV-type file
 *
 * @param filename
 */
void Statistics::save(const std::string& filename) const {
  std::ofstream fs(filename);

  std::string gen= "Generation";
  std::string spec = "SpeciesIndex";
  std::string ind= "IndividualIndex";
  std::string bestOfSpecies = "BestOfSpecies";
  std::string bestOfGen     = "BestOfGeneration";
  std::string fitness       = "Fitness";
  std::string finalFitness  = "FinalFitness";

  fs << gen << " " << spec << " " << ind << " " << bestOfSpecies << " " << bestOfGen << " ";
  fs << finalFitness << " " << fitness << std::endl;

  auto pad = [](const std::string& s, unsigned int padding) {
    if (padding >= s.size())
      return s;
    return s + std::string(" ", padding - s.size());
  };

  for(auto& generation : mEntries) {
    std::string genStr = pad(std::to_string(generation.first), gen.size());
    for(auto& entry : generation.second) {
      fs << genStr <<
        " " << pad(std::to_string(entry.speciesIndex), spec.size()) <<
        " " << pad(std::to_string(entry.individualIndex), ind.size()) <<
        " " << pad(std::to_string(entry.bestOfSpecies), bestOfSpecies.size()) <<
        " " << pad(std::to_string(entry.bestOfGeneration), bestOfGen.size()) <<
        " " << pad(std::to_string(entry.finalizedFitness), finalFitness.size()) <<
        " ";

      for (unsigned int i = 0; i < entry.individualFitness.size(); i++) {
        if (i + 1 != entry.individualFitness.size())
          fs << std::to_string(entry.individualFitness[i]) + ",";
        else
          fs << std::to_string(entry.individualFitness[i]);
      }
    }
  }
}
