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

using mmm::vec3;
using mmm::radians;

Spider::Part::Part()
    : collisionGroup(1)
    , collisionMask(-1)
    , restAngle(0)
    , active(false)
    , part(nullptr)
    , hinge(nullptr)
    , dof(nullptr) {}

Spider::Part::Part(unsigned short group,
                   unsigned short mask,
                   float          angle,
                   bool           active)
    : collisionGroup(group)
    , collisionMask(mask)
    , restAngle(angle)
    , active(active)
    , part(nullptr)
    , hinge(nullptr)
    , dof(nullptr) {}

Spider::Spider() : Logging::Log("Spider") {
  ResourceManager* r = mAsset->rManager();
  mMesh              = r->get<PhysicsMesh>("PhysicsMesh::Spider");
  mElements          = mMesh->createCopyAll();
  mParts             = SPIDER_PARTS;

  for (auto& mesh : mElements->meshes) {
    mElements->bodies[mesh.first]->setDeactivationTime(100000);
    Drawable3D* child = new MeshPart(mesh.second,
                                     mElements->bodies[mesh.first],
                                     mElements->motions[mesh.first]);

    child->setCollisionGroup(mParts[mesh.first].collisionGroup);
    child->setCollisionMask(mParts[mesh.first].collisionMask);

    for (auto& c : mElements->constraints[mesh.first]) {

      // find hinge-constraint ref for each part
      if (c->getConstraintType() ==
          btTypedConstraintType::HINGE_CONSTRAINT_TYPE) {
        auto* hinge = (btHingeConstraint*) c;

        auto*       a = &(hinge->getRigidBodyA());
        auto*       b = &(hinge->getRigidBodyB());
        std::string nameA;
        std::string nameB;

        for (auto& m : mElements->bodies) {
          if (m.second == a)
            nameA = m.first;
          if (m.second == b)
            nameB = m.first;
        }

        if (nameA == mesh.first) {
          mParts[mesh.first].hinge = hinge;
        }
      } else if (c->getConstraintType() ==
                 btTypedConstraintType::D6_SPRING_CONSTRAINT_TYPE) {

        auto* dof = (btGeneric6DofSpringConstraint*) c;

        auto*       a = &(dof->getRigidBodyA());
        auto*       b = &(dof->getRigidBodyB());
        std::string nameA;
        std::string nameB;

        for (auto& m : mElements->bodies) {
          if (m.second == a)
            nameA = m.first;
          if (m.second == b)
            nameB = m.first;
        }

        if (nameA == mesh.first) {
          mParts[mesh.first].dof = dof;
        }
      }

      child->addConstraint(c);
    }

    mParts[mesh.first].part = child;
    mChildren.push_back(child);
    child->updateFromPhysics();
  }


  if (!SPIDER_POSITIONS.size()) {
    for (auto& mesh : mElements->meshes) {
      SPIDER_POSITIONS[mesh.first] =
        mParts[mesh.first].part->rigidBody()->getWorldTransform();
    }
  }

  mLog->debug("Spider loaded");

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
  for (auto& part : mParts) {
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

std::map<std::string, Spider::Part>& Spider::parts() {
  return mParts;
}

void Spider::update(float) {}

void Spider::draw(std::shared_ptr<Program>& program, bool bindTexture) {
  draw(program, mmm::vec3(0), bindTexture);
}

/**
 * @brief
 *   Draws the spider by drawing each of the children
 *
 * @param program
 * @param offset
 * @param bindTexture
 */
void Spider::draw(std::shared_ptr<Program>& program,
                  mmm::vec3                 offset,
                  bool                      bindTexture) {
  program->bind();

  mMesh->mesh()->bindVertexArray();
  for (auto& child : mChildren)
    child->draw(program, offset, bindTexture);
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

std::map<std::string, btTransform>  Spider::SPIDER_POSITIONS = {};
std::map<std::string, Spider::Part> Spider::SPIDER_PARTS =
  { { "Abdomin",
      { 0b1000000000000000, 0b1011111111111111, radians(0), false } },
    { "Eye", { 0b1000000000000000, 0b1011111111111111, radians(0), false } },
    { "FemurL1",
      { 0b0010100000000000, 0b1001011111111111, radians(35), true } },
    { "FemurL2",
      { 0b0010010000000000, 0b1001101111111111, radians(35), true } },
    { "FemurL3",
      { 0b0010001000000000, 0b1001110111111111, radians(35), true } },
    { "FemurL4",
      { 0b0010000100000000, 0b1001111011111111, radians(35), true } },
    { "FemurR1",
      { 0b0001100000000000, 0b1010011111111111, radians(35), true } },
    { "FemurR2",
      { 0b0001010000000000, 0b1010101111111111, radians(35), true } },
    { "FemurR3",
      { 0b0001001000000000, 0b1010110111111111, radians(35), true } },
    { "FemurR4",
      { 0b0001000100000000, 0b1010111011111111, radians(35), true } },
    { "Hip", { 0b0100000000000000, 0b0011111111111111, radians(25), false } },
    { "Neck", { 0b0100000000000000, 0b0011111111111111, radians(0), false } },
    { "PatellaL1",
      { 0b0010100000000000, 0b1101011111111111, radians(-45), true } },
    { "PatellaL2",
      { 0b0010010000000000, 0b1101101111111111, radians(-45), true } },
    { "PatellaL3",
      { 0b0010001000000000, 0b1101110111111111, radians(-45), true } },
    { "PatellaL4",
      { 0b0010000100000000, 0b1101111011111111, radians(-45), true } },
    { "PatellaR1",
      { 0b0001100000000000, 0b1110011111111111, radians(-45), true } },
    { "PatellaR2",
      { 0b0001010000000000, 0b1110101111111111, radians(-45), true } },
    { "PatellaR3",
      { 0b0001001000000000, 0b1110110111111111, radians(-45), true } },
    { "PatellaR4",
      { 0b0001000100000000, 0b1110111011111111, radians(-45), true } },
    { "Sternum",
      { 0b0100000000000000, 0b1011111111111111, radians(0), false } },
    { "TarsusL1",
      { 0b0010100000000000, 0b1101011111111111, radians(-20), true } },
    { "TarsusL2",
      { 0b0010010000000000, 0b1101101111111111, radians(-20), true } },
    { "TarsusL3",
      { 0b0010001000000000, 0b1101110111111111, radians(-20), true } },
    { "TarsusL4",
      { 0b0010000100000000, 0b1101111011111111, radians(-20), true } },
    { "TarsusR1",
      { 0b0001100000000000, 0b1110011111111111, radians(-20), true } },
    { "TarsusR2",
      { 0b0001010000000000, 0b1110101111111111, radians(-20), true } },
    { "TarsusR3",
      { 0b0001001000000000, 0b1110110111111111, radians(-20), true } },
    { "TarsusR4",
      { 0b0001000100000000, 0b1110111011111111, radians(-20), true } },
    { "TibiaL1",
      { 0b0010100000000000, 0b1101011111111111, radians(-45), true } },
    { "TibiaL2",
      { 0b0010010000000000, 0b1101101111111111, radians(-45), true } },
    { "TibiaL3",
      { 0b0010001000000000, 0b1101110111111111, radians(-45), true } },
    { "TibiaL4",
      { 0b0010000100000000, 0b1101111011111111, radians(-45), true } },
    { "TibiaR1",
      { 0b0001100000000000, 0b1110011111111111, radians(-45), true } },
    { "TibiaR2",
      { 0b0001010000000000, 0b1110101111111111, radians(-45), true } },
    { "TibiaR3",
      { 0b0001001000000000, 0b1110110111111111, radians(-45), true } },
    { "TibiaR4",
      { 0b0001000100000000, 0b1110111011111111, radians(-45), true } },
    { "TrochanterL1",
      { 0b0010100000000000, 0b1001011111111111, radians(30), true } },
    { "TrochanterL2",
      { 0b0010010000000000, 0b1001101111111111, radians(10), true } },
    { "TrochanterL3",
      { 0b0010001000000000, 0b1001110111111111, radians(-15), true } },
    { "TrochanterL4",
      { 0b0010000100000000, 0b1001111011111111, radians(-40), true } },
    { "TrochanterR1",
      { 0b0001100000000000, 0b1010011111111111, radians(30), true } },
    { "TrochanterR2",
      { 0b0001010000000000, 0b1010101111111111, radians(10), true } },
    { "TrochanterR3",
      { 0b0001001000000000, 0b1010110111111111, radians(-15), true } },
    { "TrochanterR4",
      { 0b0001000100000000, 0b1010111011111111, radians(-40), true } } };
