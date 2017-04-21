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

    e.speciesId = phenotype.speciesId;
    e.speciesIndex = phenotype.speciesIndex;
    e.individualIndex = phenotype.individualIndex;
    e.finalizedFitness = phenotype.finalizedFitness;
    e.individualFitness = vecNtoList(phenotype.fitness);

    e.bestOfSpecies = leaders[phenotype.speciesIndex]->individualIndex == phenotype.individualIndex;
    e.bestOfGeneration = absoluteBest->speciesIndex == phenotype.speciesIndex &&
                         absoluteBest->individualIndex == phenotype.individualIndex;

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
  std::string id = "SpeciesID";
  std::string spec = "SpeciesIndex";
  std::string ind= "IndividualIndex";
  std::string bestOfSpecies = "BestOfSpecies";
  std::string bestOfGen     = "BestOfGeneration";
  std::string fitness       = "Fitness";
  std::string finalFitness  = "FinalFitness";

  // add csv header
  fs << gen << " " << id << " " << spec << " " << ind << " " << bestOfSpecies << " " << bestOfGen << " ";
  fs << finalFitness << " " << fitness << std::endl;

  for(auto& generation : mEntries) {
    std::string genStr = std::to_string(generation.first);
    for(auto& entry : generation.second) {
      fs << genStr <<
        " " << std::to_string(entry.speciesId) <<
        " " << std::to_string(entry.speciesIndex) <<
        " " << std::to_string(entry.individualIndex) <<
        " " << std::to_string(entry.bestOfSpecies) <<
        " " << std::to_string(entry.bestOfGeneration) <<
        " " << std::to_string(entry.finalizedFitness) <<
        " ";

      for (unsigned int i = 0; i < entry.individualFitness.size(); i++) {
        if (i + 1 != entry.individualFitness.size())
          fs << std::to_string(entry.individualFitness[i]) + ",";
        else
          fs << std::to_string(entry.individualFitness[i]);
      }

      fs << std::endl;
    }
  }
}
