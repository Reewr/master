#include "Spider.hpp"

#include <btBulletDynamicsCommon.h>

#include "../3D/Camera.hpp"
#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "MeshPart.hpp"

Spider::Part::Part()
    : collisionGroup(1), collisionMask(-1), joint(nullptr), torque(0) {}

Spider::Part::Part(unsigned short group, unsigned short mask)
    : collisionGroup(group), collisionMask(mask), joint(nullptr), torque(0) {}

Spider::Part::Part(unsigned short group, unsigned short mask, float torque)
    : collisionGroup(group)
    , collisionMask(mask)
    , joint(nullptr)
    , torque(torque) {}

Spider::Spider(Asset* asset) : Logging::Log("Spider") {
  ResourceManager* r = asset->rManager();
  mProgram           = r->get<Program>("Program::Model");
  mMesh              = r->get<PhysicsMesh>("PhysicsMesh::Spider");
  mElements          = mMesh->createCopyAll();
  mParts             = SPIDER_PARTS;

  for (auto& mesh : mElements->meshes) {
    Drawable3D* child = new MeshPart(mProgram,
                                     mesh.second,
                                     mElements->bodies[mesh.first],
                                     mElements->motions[mesh.first]);

    child->setCollisionGroup(mParts[mesh.first].collisionGroup);
    child->setCollisionMask(mParts[mesh.first].collisionMask);

    for (auto& c : mElements->constraints[mesh.first]) {
      child->addConstraint(c);
    }

    mChildren.push_back(child);
  }

  mLog->debug("Spider loaded with weight of: {}", weight());
}

Spider::~Spider() {
  for (auto& c : mChildren)
    delete c;

  mMesh->deleteCopy(mElements);
}

void Spider::update(float) {}

void Spider::drawShadow(Framebuffer* shadowMap, Camera* camera) {
  auto program = shadowMap->program();

  program->bind();
  camera->setLightVPUniforms(program, "light");

  mMesh->mesh()->bindVertexArray();
  for (auto& child : mChildren)
    child->drawShadow(shadowMap, camera);
  mMesh->mesh()->unbindVertexArray();
}
void Spider::draw(Camera* c) {
  mProgram->bind();

  // mmm::mat4 model = mmm::translate(mPosition) * mRotation * mScale;

  mProgram->setUniform("view", c->view());
  mProgram->setUniform("proj", c->projection());

  mProgram->setUniform("dir", c->light().direction);
  c->setLightVPUniforms(mProgram, "light");

  mMesh->mesh()->bindVertexArray();
  for (auto& child : mChildren)
    child->draw(c);
  mMesh->mesh()->unbindVertexArray();
}

void Spider::input(const Input::Event&) {}

std::map<std::string, Spider::Part> Spider::SPIDER_PARTS =
  { { "Eye", { 0b1000000000000000, 0b1011111111111111 } },
    { "Neck", { 0b0100000000000000, 0b0011111111111111 } },
    { "Coxa1", { 0b0100100000000000, 0b1000011111111111 } },
    { "ThoraxFront", { 0b0011000000000000, 0b1000111111111111 } },
    { "Coxa2", { 0b0100010000000000, 0b1000101111111111 } },
    { "SternumFront", { 0b0100000000000000, 0b1011111111111111 } },
    { "SternumBack", { 0b0100000000000000, 0b1011111111111111 } },
    { "Coxa3", { 0b0100001000000000, 0b1000110111111111 } },
    { "ThoraxBack", { 0b0011000000000000, 0b1000111111111111 } },
    { "Coxa4", { 0b0100000100000000, 0b1000111011111111 } },
    { "Hip", { 0b0100000000000000, 0b0011111111111111 } },
    { "Abdomin", { 0b1000000000000000, 0b1011111111111111 } },

    { "TrochanterL1", { 0b0010100000000000, 0b1001011111111111 } },
    { "FemurL1", { 0b0010100000000000, 0b1001011111111111 } },
    { "PatellaL1", { 0b0010100000000000, 0b1101011111111111 } },
    { "TibiaL1", { 0b0010100000000000, 0b1101011111111111 } },
    { "MetatarsusL1", { 0b0010100000000000, 0b1101011111111111 } },
    { "TarsusL1", { 0b0010100000000000, 0b1101011111111111 } },

    { "TrochanterL2", { 0b0010010000000000, 0b1001101111111111 } },
    { "FemurL2", { 0b0010010000000000, 0b1001101111111111 } },
    { "PatellaL2", { 0b0010010000000000, 0b1101101111111111 } },
    { "TibiaL2", { 0b0010010000000000, 0b1101101111111111 } },
    { "MetatarsusL2", { 0b0010010000000000, 0b1101101111111111 } },
    { "TarsusL2", { 0b0010010000000000, 0b1101101111111111 } },

    { "TrochanterL3", { 0b0010001000000000, 0b1001110111111111 } },
    { "FemurL3", { 0b0010001000000000, 0b1001110111111111 } },
    { "PatellaL3", { 0b0010001000000000, 0b1101110111111111 } },
    { "TibiaL3", { 0b0010001000000000, 0b1101110111111111 } },
    { "MetatarsusL3", { 0b0010001000000000, 0b1101110111111111 } },
    { "TarsusL3", { 0b0010001000000000, 0b1101110111111111 } },

    { "TrochanterL4", { 0b0010000100000000, 0b1001111011111111 } },
    { "FemurL4", { 0b0010000100000000, 0b1001111011111111 } },
    { "PatellaL4", { 0b0010000100000000, 0b1101111011111111 } },
    { "TibiaL4", { 0b0010000100000000, 0b1101111011111111 } },
    { "MetatarsusL4", { 0b0010000100000000, 0b1101111011111111 } },
    { "TarsusL4", { 0b0010000100000000, 0b1101111011111111 } },

    { "TrochanterR1", { 0b0001100000000000, 0b1010011111111111 } },
    { "FemurR1", { 0b0001100000000000, 0b1010011111111111 } },
    { "PatellaR1", { 0b0001100000000000, 0b1110011111111111 } },
    { "TibiaR1", { 0b0001100000000000, 0b1110011111111111 } },
    { "MetatarsusR1", { 0b0001100000000000, 0b1110011111111111 } },
    { "TarsusR1", { 0b0001100000000000, 0b1110011111111111 } },

    { "TrochanterR2", { 0b0001010000000000, 0b1010101111111111 } },
    { "FemurR2", { 0b0001010000000000, 0b1010101111111111 } },
    { "PatellaR2", { 0b0001010000000000, 0b1110101111111111 } },
    { "TibiaR2", { 0b0001010000000000, 0b1110101111111111 } },
    { "MetatarsusR2", { 0b0001010000000000, 0b1110101111111111 } },
    { "TarsusR2", { 0b0001010000000000, 0b1110101111111111 } },

    { "TrochanterR3", { 0b0001001000000000, 0b1010110111111111 } },
    { "FemurR3", { 0b0001001000000000, 0b1010110111111111 } },
    { "PatellaR3", { 0b0001001000000000, 0b1110110111111111 } },
    { "TibiaR3", { 0b0001001000000000, 0b1110110111111111 } },
    { "MetatarsusR3", { 0b0001001000000000, 0b1110110111111111 } },
    { "TarsusR3", { 0b0001001000000000, 0b1110110111111111 } },

    { "TrochanterR4", { 0b0001000100000000, 0b1010111011111111 } },
    { "FemurR4", { 0b0001000100000000, 0b1010111011111111 } },
    { "PatellaR4", { 0b0001000100000000, 0b1110111011111111 } },
    { "TibiaR4", { 0b0001000100000000, 0b1110111011111111 } },
    { "MetatarsusR4", { 0b0001000100000000, 0b1110111011111111 } },
    { "TarsusR4", { 0b0001000100000000, 0b1110111011111111 } } };
