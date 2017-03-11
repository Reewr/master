#include "Spider.hpp"

#include <btBulletDynamicsCommon.h>

#include "../3D/Camera.hpp"
#include "../GLSL/Program.hpp"
#include "../Resource/PhysicsMesh.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "MeshPart.hpp"

std::string toString(SpiderPart p) {
  switch (p) {
    case Abdomen:
      return "Abdomen";
    case AbdomenInner:
      return "AbdomenInner";
    case Coxa:
      return "Coxa";
    case Femur:
      return "Femur";
    case FemurTibiaInner:
      return "FemurTibiaInner";
    case Head:
      return "Head";
    case HeadDisk:
      return "HeadDisk";
    case HeadInner:
      return "HeadInner";
    case Hip:
      return "Hip";
    case Joint:
      return "Joint";
    case JointRot:
      return "JointRot";
    case Metatarsus:
      return "Metatarsus";
    case Neck:
      return "Neck";
    case Patella:
      return "Patella";
    case PatellaInner:
      return "PatellaInner";
    case SternumBack:
      return "SternumBack";
    case SternumFront:
      return "SternumFront";
    case TarsusBot:
      return "TarsusBot";
    case TarsusTop:
      return "TarsusTop";
    case Thorax:
      return "Thorax";
    case Tibia:
      return "Tibia";
    case Trochanter:
      return "Trochanter";
  }
}

Spider::Spider(Asset* asset)
  : Logging::Log("Spider") {
  ResourceManager* r = asset->rManager();
  mProgram = r->get<Program>("Program::Model");
  mMesh = r->get<PhysicsMesh>("PhysicsMesh::Box");
  mElements = mMesh->createCopyAll();

  for(auto& mesh : mElements->meshes) {
    mLog->debug("Adding MeshPart: {}", mesh.first);
    mChildren.push_back(new MeshPart(mProgram,
                                     mesh.second,
                                     mElements->bodies[mesh.first],
                                     mElements->motions[mesh.first]));

    for(auto& c : mElements->constraints[mesh.first]) {
      mChildren.back()->addConstraint(c);
    }
  }

  // for (auto subMesh : allSubmeshes) {
  //   mLog->debug("Adding MeshPart: {}", subMesh.first);
  //   mChildren.push_back(new MeshPart(mProgram, subMesh.second));
  // }
}

Spider::~Spider() {
  mMesh->deleteCopy(mElements);
}

void Spider::update(float) {}

void Spider::drawShadow(Framebuffer*, Camera*) {}
void Spider::draw(Camera* c) {
  mProgram->bind();

  // mmm::mat4 model = mmm::translate(mPosition) * mRotation * mScale;

  mProgram->setUniform("view", c->view());
  mProgram->setUniform("proj", c->projection());

  mProgram->setUniform("dir", c->light().direction);
  c->setLightVPUniforms(mProgram, "light");

  mMesh->mesh()->bindVertexArray();
  for(auto& child : mChildren)
    child->draw(c);
  mMesh->mesh()->unbindVertexArray();
}

void Spider::input(const Input::Event&) {}
