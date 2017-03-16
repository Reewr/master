#pragma once

#include <map>
#include <btBulletDynamicsCommon.h>

class World;
class Spider;

class SpiderSwarm {
public:

  struct NonSpiderCollisionFilter : public btOverlapFilterCallback {
    bool needBroadphaseCollision(btBroadphaseProxy* a,
                                 btBroadphaseProxy* b) const;
  };

  SpiderSwarm(World* world);
  ~SpiderSwarm();

  Spider* addSpider();
  bool removeSpider(Spider* spider);
  bool removeSpider(int id);

  Spider* spider(int id);

  const std::map<int, Spider*>& spiders();
  const std::map<Spider*, int>& ids();

private:
  std::map<int, Spider*> mSpiders;
  std::map<Spider*, int> mSpiderIds;
  World* mWorld;

  static int mBodyIds;
};
