#include "SpiderSwarm.hpp"

#include "../3D/World.hpp"
#include "../3D/Spider.hpp"

#include <btBulletDynamicsCommon.h>

/**
 * @brief
 *   For simulation purposes, the spiders should not collide with each other
 *   as they are technically suppose to live in their own world.
 *
 *   In order to save memory, we add all the spiders to the same world, but
 *   use this function to ignore any collision between spiders.
 *
 *   By default, only filter group and mask is taken into consideration
 *   when evaulation whether or not to perform collision detection on an object.
 *
 *   In addition to the default behaviour, the function also checks to see if
 *   the rigid bodies have the same parent. If this is the case, normal
 *   collision detection is performed. If they do not have the same parent,
 *   they should never be able to collide
 *
 * @param a
 * @param b
 *
 * @return
 */
bool SpiderSwarm::NonSpiderCollisionFilter::needBroadphaseCollision (
    btBroadphaseProxy* a,
    btBroadphaseProxy* b) const {
  btCollisionObject* aObj = (btCollisionObject*) a->m_clientObject;
  btCollisionObject* bObj = (btCollisionObject*) b->m_clientObject;

  bool collides = (a->m_collisionFilterGroup & b->m_collisionFilterMask) != 0;
  collides = collides && (a->m_collisionFilterGroup & b->m_collisionFilterMask);

  if (!aObj || !bObj) {
    return collides;
  }

  Spider* aSpider = (Spider*) aObj->getUserPointer();
  Spider* bSpider = (Spider*) bObj->getUserPointer();

  if (!aSpider || !bSpider) {
    return collides;
  }

  if (aSpider == bSpider)
    return collides;

  return false;
}

int SpiderSwarm::mBodyIds = 0;

SpiderSwarm::SpiderSwarm(World* world) : mWorld(world) {
  btOverlapFilterCallback* filter = new NonSpiderCollisionFilter();

  mWorld->setCollisionFilter(filter);
}

/**
 * @brief
 *   Deletes all the spiders after deattaching them from the world.
 */
SpiderSwarm::~SpiderSwarm() {
  for (auto& c : mSpiders) {
    mWorld->removeObject(c.second);
    delete c.second;
  }

  mSpiders.clear();
  mSpiderIds.clear();
}

/**
 * @brief
 *   Adds a new spider to the list of spiders as well as to the world. Each
 *   added spider gets a unique Id that is added to each of the rigid bodies.
 *   In addition, each child of spider gets a pointer to the spider object.
 *
 * @return
 */
Spider* SpiderSwarm::addSpider() {
  int     id     = ++mBodyIds;
  Spider* spider = new Spider();

  mSpiders[id] = spider;
  mSpiderIds[spider] = id;

  for (auto& child : spider->children()) {
    child->rigidBody()->setUserIndex(id);
    child->rigidBody()->setUserPointer(spider);
  }

  mWorld->addObject(spider);

  return spider;
}

/**
 * @brief
 *   Removes a spider indicated by a pointer to the object, removing it
 *   from storage, the world and deleting the allocated object
 *
 * @param spider
 *
 * @return
 */
bool SpiderSwarm::removeSpider(Spider* spider) {
  if (!mSpiderIds.count(spider))
    return false;

  int id = mSpiderIds[spider];

  mSpiderIds.erase(spider);
  mSpiders.erase(id);

  mWorld->removeObject(spider);

  delete spider;

  return true;
}

/**
 * @brief
 *   Removes a spider indicated by an id, removing it
 *   from storage, the world and deleting the allocated object
 *
 * @param id
 *
 * @return
 */
bool SpiderSwarm::removeSpider(int id) {
  if (id < 0 || mSpiders.count(id))
    return false;

  return removeSpider(mSpiders[id]);
}

/**
 * @brief
 *   Retrieves a spider by id, returning a nullptr if it does not exist
 *   otherwise returning a pointer to the spider
 *
 * @param id
 *
 * @return
 */
Spider* SpiderSwarm::spider(int id) {
  if (!mSpiders.count(id))
    return nullptr;
  return mSpiders[id];
}

/**
 * @brief
 *   Returns a const reference to all the spiders
 *
 * @return
 */
const std::map<int, Spider*>& SpiderSwarm::spiders() {
  return mSpiders;
}

/**
 * @brief
 *   Returns a const reference to all the ids
 *
 * @return
 */
const std::map<Spider*, int>& SpiderSwarm::ids() {
  return mSpiderIds;
}
