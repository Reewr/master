#include "Spider.hpp"

#include <btBulletDynamicsCommon.h>

#include "../Camera/Camera.hpp"
#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "MeshPart.hpp"

Spider::Part::Part()
    : collisionGroup(1), collisionMask(-1), part(nullptr) {}

Spider::Part::Part(unsigned short group, unsigned short mask)
    : collisionGroup(group)
    , collisionMask(mask)
    , part(nullptr) {}

Spider::Spider() : Logging::Log("Spider") {
  ResourceManager* r = mAsset->rManager();
  mMesh              = r->get<PhysicsMesh>("PhysicsMesh::Spider");
  mElements          = mMesh->createCopyAll();
  mParts             = SPIDER_PARTS;

  for (auto& mesh : mElements->meshes) {
    Drawable3D* child = new MeshPart(mesh.second,
                                     mElements->bodies[mesh.first],
                                     mElements->motions[mesh.first]);

    child->setCollisionGroup(mParts[mesh.first].collisionGroup);
    child->setCollisionMask(mParts[mesh.first].collisionMask);

    for (auto& c : mElements->constraints[mesh.first]) {
      child->addConstraint(c);
    }

    mParts[mesh.first].part = child;
    mChildren.push_back(child);
    child->updateFromPhysics();
  }


  if (!SPIDER_POSITIONS.size()) {
    for (auto& mesh : mElements->meshes) {
      SPIDER_POSITIONS[mesh.first] = mParts[mesh.first].part->rigidBody()->getWorldTransform();
    }
  }

  mLog->debug("Spider loaded with weight of: {}", weight());

  reset();
}

Spider::~Spider() {
  for (auto& c : mChildren)
    delete c;

  mMesh->deleteCopy(mElements);
}

/**
 * @brief
 *   Resets the spiders position to the start state
 */
void Spider::reset() {
  btVector3 zero(0, 0, 0);
  for(auto& part : mParts) {
    btRigidBody* r = part.second.part->rigidBody();
    r->clearForces();
    r->setAngularVelocity(zero);
    r->setLinearVelocity(zero);
    r->setWorldTransform(SPIDER_POSITIONS[part.first]);
    r->activate(true);
  }
}

/**
 * @brief
 *   Looks up into the children and checks for a child with the specific name.
 *   If one is found, a pointer to that one is returned.
 *
 *   If no child is found, a nullptr is returned
 *
 * @param name
 *
 * @return
 */
Drawable3D* Spider::child(const std::string& name) {
  if (mParts.count(name) == 0)
    return nullptr;
  return mParts[name].part;
}

void Spider::update(float) {}

/**
 * @brief
 *   Draws the spider by drawing each of the children
 *
 * @param program
 * @param bindTexture
 */
void Spider::draw(std::shared_ptr<Program>& program, bool bindTexture) {
  program->bind();

  mMesh->mesh()->bindVertexArray();
  for (auto& child : mChildren)
    child->draw(program, bindTexture);
  mMesh->mesh()->unbindVertexArray();
}

void Spider::input(const Input::Event&) {}

/**
 * @brief
 *   Tries to upcast a Drawable3D object to a spider, returning a
 *   casted pointer if it works, otherwise nullptr
 *
 * @param drawable
 *
 * @return
 */
Spider* Spider::upcast(Drawable3D* drawable) {
  if (drawable == nullptr)
    return nullptr;

  return dynamic_cast<Spider*>(drawable);
}

std::map<std::string, btTransform> Spider::SPIDER_POSITIONS = {};
std::map<std::string, Spider::Part> Spider::SPIDER_PARTS =
  { { "Eye", { 0b1000000000000000, 0b1011111111111111 } },
    { "Neck", { 0b0100000000000000, 0b0011111111111111 } },
    // { "Coxa1", { 0b0100100000000000, 0b1000011111111111 } },
    // { "ThoraxFront", { 0b0011000000000000, 0b1000111111111111 } },
    // { "Coxa2", { 0b0100010000000000, 0b1000101111111111 } },
    // { "SternumFront", { 0b0100000000000000, 0b1011111111111111 } },
    { "Sternum", { 0b0100000000000000, 0b1011111111111111 } },
    // { "SternumBack", { 0b0100000000000000, 0b1011111111111111 } },
    // { "Coxa3", { 0b0100001000000000, 0b1000110111111111 } },
    // { "ThoraxBack", { 0b0011000000000000, 0b1000111111111111 } },
    // { "Coxa4", { 0b0100000100000000, 0b1000111011111111 } },
    { "Hip", { 0b0100000000000000, 0b0011111111111111 } },
    { "Abdomin", { 0b1000000000000000, 0b1011111111111111 } },

    { "TrochanterL1", { 0b0010100000000000, 0b1001011111111111 } },
    { "FemurL1", { 0b0010100000000000, 0b1001011111111111 } },
    { "PatellaL1", { 0b0010100000000000, 0b1101011111111111 } },
    { "TibiaL1", { 0b0010100000000000, 0b1101011111111111 } },
    // { "MetatarsusL1", { 0b0010100000000000, 0b1101011111111111 } },
    { "TarsusL1", { 0b0010100000000000, 0b1101011111111111 } },

    { "TrochanterL2", { 0b0010010000000000, 0b1001101111111111 } },
    { "FemurL2", { 0b0010010000000000, 0b1001101111111111 } },
    { "PatellaL2", { 0b0010010000000000, 0b1101101111111111 } },
    { "TibiaL2", { 0b0010010000000000, 0b1101101111111111 } },
    // { "MetatarsusL2", { 0b0010010000000000, 0b1101101111111111 } },
    { "TarsusL2", { 0b0010010000000000, 0b1101101111111111 } },

    { "TrochanterL3", { 0b0010001000000000, 0b1001110111111111 } },
    { "FemurL3", { 0b0010001000000000, 0b1001110111111111 } },
    { "PatellaL3", { 0b0010001000000000, 0b1101110111111111 } },
    { "TibiaL3", { 0b0010001000000000, 0b1101110111111111 } },
    // { "MetatarsusL3", { 0b0010001000000000, 0b1101110111111111 } },
    { "TarsusL3", { 0b0010001000000000, 0b1101110111111111 } },

    { "TrochanterL4", { 0b0010000100000000, 0b1001111011111111 } },
    { "FemurL4", { 0b0010000100000000, 0b1001111011111111 } },
    { "PatellaL4", { 0b0010000100000000, 0b1101111011111111 } },
    { "TibiaL4", { 0b0010000100000000, 0b1101111011111111 } },
    // { "MetatarsusL4", { 0b0010000100000000, 0b1101111011111111 } },
    { "TarsusL4", { 0b0010000100000000, 0b1101111011111111 } },

    { "TrochanterR1", { 0b0001100000000000, 0b1010011111111111 } },
    { "FemurR1", { 0b0001100000000000, 0b1010011111111111 } },
    { "PatellaR1", { 0b0001100000000000, 0b1110011111111111 } },
    { "TibiaR1", { 0b0001100000000000, 0b1110011111111111 } },
    // { "MetatarsusR1", { 0b0001100000000000, 0b1110011111111111 } },
    { "TarsusR1", { 0b0001100000000000, 0b1110011111111111 } },

    { "TrochanterR2", { 0b0001010000000000, 0b1010101111111111 } },
    { "FemurR2", { 0b0001010000000000, 0b1010101111111111 } },
    { "PatellaR2", { 0b0001010000000000, 0b1110101111111111 } },
    { "TibiaR2", { 0b0001010000000000, 0b1110101111111111 } },
    // { "MetatarsusR2", { 0b0001010000000000, 0b1110101111111111 } },
    { "TarsusR2", { 0b0001010000000000, 0b1110101111111111 } },

    { "TrochanterR3", { 0b0001001000000000, 0b1010110111111111 } },
    { "FemurR3", { 0b0001001000000000, 0b1010110111111111 } },
    { "PatellaR3", { 0b0001001000000000, 0b1110110111111111 } },
    { "TibiaR3", { 0b0001001000000000, 0b1110110111111111 } },
    // { "MetatarsusR3", { 0b0001001000000000, 0b1110110111111111 } },
    { "TarsusR3", { 0b0001001000000000, 0b1110110111111111 } },

    { "TrochanterR4", { 0b0001000100000000, 0b1010111011111111 } },
    { "FemurR4", { 0b0001000100000000, 0b1010111011111111 } },
    { "PatellaR4", { 0b0001000100000000, 0b1110111011111111 } },
    { "TibiaR4", { 0b0001000100000000, 0b1110111011111111 } },
    // { "MetatarsusR4", { 0b0001000100000000, 0b1110111011111111 } },
    { "TarsusR4", { 0b0001000100000000, 0b1110111011111111 } } };
