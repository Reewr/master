#include "SpiderSwarm.hpp"

#include "../3D/World.hpp"
#include "../3D/Spider.hpp"

#include <btBulletDynamicsCommon.h>
#include <NeuralNetwork.h>

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
  bool collides = (a->m_collisionFilterGroup & b->m_collisionFilterMask) != 0 &&
                  (b->m_collisionFilterGroup & a->m_collisionFilterMask);

  // If collides is false, dont check anything else
  if (!collides)
    return false;

  btCollisionObject* aObj = (btCollisionObject*) a->m_clientObject;
  btCollisionObject* bObj = (btCollisionObject*) b->m_clientObject;

  if (!aObj || !bObj) {
    return true;
  }

  int aId = aObj->getUserIndex();
  int bId = bObj->getUserIndex();

  if (aId == -1 || bId == -1) {
    return true;
  }

  if (aId == bId) {
    return true;
  }

  return false;
}

int SpiderSwarm::mBodyIds = 0;

SpiderSwarm::SpiderSwarm() {
  mNetwork = new NEAT::NeuralNetwork();
}

/**
 * @brief
 *   Deletes all the spiders after deattaching them from the world.
 */
SpiderSwarm::~SpiderSwarm() {
  for (auto& c : mSpiders) {
    removeSpider(c.first);
  }

  mSpiders.clear();
  delete mNetwork;
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
  int     id      = ++mBodyIds;
  Spider* spider  = new Spider();
  World*  world   = new World(mmm::vec3(0, -9.81, 0));

  mSpiders[id] = {world, spider};

  for (auto& child : spider->children()) {
    child->rigidBody()->setUserIndex(id);
  }

  world->addObject(spider);

  return spider;
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

  mSpiders[id].world->removeObject(mSpiders[id].spider);

  delete mSpiders[id].spider;
  delete mSpiders[id].world;

  return true;
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
  return mSpiders[id].spider;
}

/**
 * @brief
 *   Returns a const reference to all the spiders
 *
 * @return
 */
const std::map<int, SpiderSwarm::SpiderWorld>& SpiderSwarm::spiders() {
  return mSpiders;
}

/**
 * @brief
 *   Goes through all the stored spiders and worlds and updates them
 */
void SpiderSwarm::update(float deltaTime) {
  for(auto& spiderWorld : mSpiders) {
    spiderWorld.second.world->doPhysics(deltaTime);
  }
}
