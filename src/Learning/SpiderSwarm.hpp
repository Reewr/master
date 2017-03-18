#pragma once

#include <map>
#include <btBulletDynamicsCommon.h>

class World;
class Spider;
class Terrain;

namespace NEAT {
  class NeuralNetwork;
}

class SpiderSwarm {
public:

  struct NonSpiderCollisionFilter : public btOverlapFilterCallback {
    bool needBroadphaseCollision(btBroadphaseProxy* a,
                                 btBroadphaseProxy* b) const;
  };

  struct SpiderWorld {
    World* world;
    Spider* spider;
  };

  SpiderSwarm();
  ~SpiderSwarm();

  Spider* addSpider();
  bool removeSpider(int id);

  Spider* spider(int id);

  const std::map<int, SpiderWorld>& spiders();

  void update(float deltaTime);

private:
  std::map<int, SpiderWorld> mSpiders;
  NEAT::NeuralNetwork* mNetwork;

  static int mBodyIds;
};
