#pragma once

#include <vector>

#include <btBulletDynamicsCommon.h>
#include <mmm.hpp>

#include "../Log.hpp"

class World;
class Spider;
class Terrain;

namespace NEAT {
  class NeuralNetwork;
  class Parameters;
  class Population;
  class Substrate;
}

class SpiderSwarm : Logging::Log {
public:

  struct NonSpiderCollisionFilter : public btOverlapFilterCallback {
    bool needBroadphaseCollision(btBroadphaseProxy* a,
                                 btBroadphaseProxy* b) const;
  };

  struct Phenotype {
    World*               world;
    Spider*              spider;
    NEAT::NeuralNetwork* network;
    mmm::vec<8>          fitness;

    Phenotype();
    ~Phenotype();
  };

  SpiderSwarm();
  // for loading a population from file(s)
  // SpiderSwarm(NEAT::Parameters*, NEAT::Substrate*);
  ~SpiderSwarm();

  void update(float deltaTime);

private:

  std::vector<Phenotype> mPhenotypes;

  size_t mCurrentBatch;
  size_t mBatchStart;
  size_t mBatchEnd;
  size_t mBatchSize;

  float  mCurrentDuration;
  float  mIterationDuration;

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
