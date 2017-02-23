#include "Spider.hpp"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <btBulletDynamicsCommon.h>

#include "../3D/Camera.hpp"
#include "../GLSL/Program.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"

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

Spider::Spider(Asset* asset) : Logging::Log("Spider") {
  ResourceManager* r = asset->rManager();

  mMeshes[SpiderPart::Abdomen]      = r->get<Mesh>("Mesh::Abdomen");
  mMeshes[SpiderPart::AbdomenInner] = r->get<Mesh>("Mesh::AbdomenInner");

  btConvexHullShape* shape = new btConvexHullShape();
  const auto&        data  = mMeshes[SpiderPart::Abdomen]->data();

  for (size_t i = 0; i < data.size(); i += 8) {
    shape->addPoint(btVector3(data[i], data[i + 1], data[i + 2]));
  }

  btShapeHull* hull               = new btShapeHull(shape);
  btConvexHullShape* reducedShape = new btConvexHullShape();
  hull->buildHull(shape->getMargin());

  for (int i = 0; i < hull->numVertices(); ++i) {
    reducedShape->addPoint(hull->getVertexPointer()[i]);
  }

  mShape = reducedShape;

  delete hull;
  delete shape;

  mMotion = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.f, 0.f, 0.f)));

  btScalar mass = 1000;

  btVector3 fallInertia(0, 0, 0);
  mShape->calculateLocalInertia(mass, fallInertia);
  btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
                                                    mMotion,
                                                    mShape,
                                                    fallInertia);
  mBody    = new btRigidBody(consInfo);
  mProgram = r->get<Program>("Program::Model");
}

Spider::~Spider() {
  delete mBody;
  delete mShape;
  delete mMotion;
}

void Spider::update(float) {}

void Spider::drawShadow(Framebuffer*, Camera*) {}
void Spider::draw(Camera* c) {
  mProgram->bind();

  mmm::mat4 model = mmm::translate(mPosition) * mRotation * mScale;

  mProgram->setUniform("model", model);
  mProgram->setUniform("view", c->view());
  mProgram->setUniform("proj", c->projection());

  mProgram->setUniform("dir", c->light().direction);
  c->setLightVPUniforms(mProgram, "light");

  for (auto& mesh : mMeshes) {
    mesh.second->draw(model, mProgram);
  }
}

void Spider::input(const Input::Event&) {}
