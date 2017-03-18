#pragma once

#include <map>
#include <btBulletDynamicsCommon.h>

#include <mmm.hpp>

class World;
class Spider;
class Terrain;

namespace NEAT {
  class NeuralNetwork;
  class Parameters;
  class Population;
  class Substrate;
}

class SpiderSwarm {
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
  };

  SpiderSwarm();
  // for loading a population from file(s)
  // SpiderSwarm(NEAT::Parameters*, NEAT::Substrate*);
  ~SpiderSwarm();

  Spider* addSpider();
  bool removeSpider(int id);

  Spider* spider(int id);

  const std::map<int, Phenotype>& spiders();

  void update(float deltaTime);

private:
  std::map<int, Phenotype> mSpiders;

  static int mBodyIds;

  NEAT::Parameters* mParameters;
  NEAT::Substrate*  mSubstrate;
  NEAT::Population* mPopulation;

  void setDefaultParameters();
  void setDefaultSubstrate();
  void setDefaultPopulation();
};
