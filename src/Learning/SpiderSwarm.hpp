#pragma once

#include <memory>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../Log.hpp"

class Program;
class Spider;
class Terrain;
class World;

namespace NEAT {
  class NeuralNetwork;
  class Parameters;
  class Population;
  class Substrate;
}

class SpiderSwarm : Logging::Log {
public:

  struct Phenotype {
    World*               world;
    Spider*              spider;
    NEAT::NeuralNetwork* network;
    mmm::vec<8>          fitness;

    Phenotype();
    ~Phenotype();
  };

  SpiderSwarm();
  ~SpiderSwarm();

  void update(float deltaTime);
  void draw(std::shared_ptr<Program>& prog, bool bindTexture);

private:

  std::vector<Phenotype> mPhenotypes;

  size_t mCurrentBatch;
  size_t mBatchStart;
  size_t mBatchEnd;
  size_t mBatchSize;

  float  mCurrentDuration;
  float  mIterationDuration;
  size_t mDrawLimit;

  void updateNormal(float deltaTime);
  void updateBatch();
  void updateEpoch();
  void recreatePhenotypes();

  // NEAT stuff
  NEAT::Parameters* mParameters;
  NEAT::Substrate*  mSubstrate;
  NEAT::Population* mPopulation;

  void setDefaultParameters();
  void setDefaultSubstrate();
  void setDefaultPopulation();
};
