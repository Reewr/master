#include "Spider.hpp"

#include <btBulletDynamicsCommon.h>

#include "../3D/Camera.hpp"
#include "../GLSL/Program.hpp"
#include "../Resource/Mesh.hpp"
#include "../Resource/PhysicsMesh.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "MeshPart.hpp"

// std::string toString(SpiderPart p) {
//   switch (p) {
//     case Abdomen:
//       return "Abdomen";
//     case AbdomenInner:
//       return "AbdomenInner";
//     case Coxa:
//       return "Coxa";
//     case Femur:
//       return "Femur";
//     case FemurTibiaInner:
//       return "FemurTibiaInner";
//     case Head:
//       return "Head";
//     case HeadDisk:
//       return "HeadDisk";
//     case HeadInner:
//       return "HeadInner";
//     case Hip:
//       return "Hip";
//     case Joint:
//       return "Joint";
//     case JointRot:
//       return "JointRot";
//     case Metatarsus:
//       return "Metatarsus";
//     case Neck:
//       return "Neck";
//     case Patella:
//       return "Patella";
//     case PatellaInner:
//       return "PatellaInner";
//     case SternumBack:
//       return "SternumBack";
//     case SternumFront:
//       return "SternumFront";
//     case TarsusBot:
//       return "TarsusBot";
//     case TarsusTop:
//       return "TarsusTop";
//     case Thorax:
//       return "Thorax";
//     case Tibia:
//       return "Tibia";
//     case Trochanter:
//       return "Trochanter";
//   }
// }

enum SCol {
  Special = 2,
  Left    = 4,
  Center  = 8,
  Right   = 16,
  Level1  = 2048,
  Level2  = 4096,
  Level3  = 8192,
  Level4  = 16384
};

static std::map<std::string, std::pair<short, short>> SColFilters = {
  {"Eye",           {SCol::Special, -1 & SCol::Center}},
  {"Abdomin",       {SCol::Special, -1 & SCol::Center}},
  {"Neck",          {SCol::Center, -1 & SCol::Center & SCol::Special}},
  {"Hip",           {SCol::Center, -1 & SCol::Center & SCol::Special}},
  {"Coxa1",         {SCol::Center, -1 & SCol::Left & SCol::Center & SCol::Right & SCol::Level1}},
  {"Coxa2",         {SCol::Center, -1 & SCol::Left & SCol::Center & SCol::Right & SCol::Level2}},
  {"Coxa3",         {SCol::Center, -1 & SCol::Left & SCol::Center & SCol::Right & SCol::Level3}},
  {"Coxa4",         {SCol::Center, -1 & SCol::Left & SCol::Center & SCol::Right & SCol::Level4}},
  {"SternumFront",  {SCol::Center, -1 & SCol::Center}},
  {"SternumBack",   {SCol::Center, -1 & SCol::Center}},
  {"ThoraxFront",   {SCol::Left & SCol::Right, -1 & SCol::Center}},
  {"ThoraxBack",    {SCol::Left & SCol::Right, -1 & SCol::Center}},
  {"TrochanterL1",  {SCol::Left & SCol::Level1, -1 & SCol::Center & SCol::Level1}},
  {"FemurL1",       {SCol::Left & SCol::Level1, -1 & SCol::Center & SCol::Level1}},
  {"PatellaL1",     {SCol::Left & SCol::Level1, -1 & SCol::Level1}},
  {"TibiaL1",       {SCol::Left & SCol::Level1, -1 & SCol::Level1}},
  {"MetatarsusL1",  {SCol::Left & SCol::Level1, -1 & SCol::Level1}},
  {"TarsusL1",      {SCol::Left & SCol::Level1, -1 & SCol::Level1}},
  {"TrochanterL2",  {SCol::Left & SCol::Level2, -1 & SCol::Center & SCol::Level2}},
  {"FemurL2",       {SCol::Left & SCol::Level2, -1 & SCol::Center & SCol::Level2}},
  {"PatellaL2",     {SCol::Left & SCol::Level2, -1 & SCol::Level2}},
  {"TibiaL2",       {SCol::Left & SCol::Level2, -1 & SCol::Level2}},
  {"MetatarsusL2",  {SCol::Left & SCol::Level2, -1 & SCol::Level2}},
  {"TarsusL2",      {SCol::Left & SCol::Level2, -1 & SCol::Level2}},
  {"TrochanterL3",  {SCol::Left & SCol::Level3, -1 & SCol::Center & SCol::Level3}},
  {"FemurL3",       {SCol::Left & SCol::Level3, -1 & SCol::Center & SCol::Level3}},
  {"PatellaL3",     {SCol::Left & SCol::Level3, -1 & SCol::Level3}},
  {"TibiaL3",       {SCol::Left & SCol::Level3, -1 & SCol::Level3}},
  {"MetatarsusL3",  {SCol::Left & SCol::Level3, -1 & SCol::Level3}},
  {"TarsusL3",      {SCol::Left & SCol::Level3, -1 & SCol::Level3}},
  {"TrochanterL4",  {SCol::Left & SCol::Level4, -1 & SCol::Center & SCol::Level4}},
  {"FemurL4",       {SCol::Left & SCol::Level4, -1 & SCol::Center & SCol::Level4}},
  {"PatellaL4",     {SCol::Left & SCol::Level4, -1 & SCol::Level4}},
  {"TibiaL4",       {SCol::Left & SCol::Level4, -1 & SCol::Level4}},
  {"MetatarsusL4",  {SCol::Left & SCol::Level4, -1 & SCol::Level4}},
  {"TarsusL4",      {SCol::Left & SCol::Level4, -1 & SCol::Level4}},
  {"TrochanterR1",  {SCol::Right & SCol::Level1, -1 & SCol::Center & SCol::Level1}},
  {"FemurR1",       {SCol::Right & SCol::Level1, -1 & SCol::Center & SCol::Level1}},
  {"PatellaR1",     {SCol::Right & SCol::Level1, -1 & SCol::Level1}},
  {"TibiaR1",       {SCol::Right & SCol::Level1, -1 & SCol::Level1}},
  {"MetatarsusR1",  {SCol::Right & SCol::Level1, -1 & SCol::Level1}},
  {"TarsusR1",      {SCol::Right & SCol::Level1, -1 & SCol::Level1}},
  {"TrochanterR2",  {SCol::Right & SCol::Level2, -1 & SCol::Center & SCol::Level2}},
  {"FemurR2",       {SCol::Right & SCol::Level2, -1 & SCol::Center & SCol::Level2}},
  {"PatellaR2",     {SCol::Right & SCol::Level2, -1 & SCol::Level2}},
  {"TibiaR2",       {SCol::Right & SCol::Level2, -1 & SCol::Level2}},
  {"MetatarsusR2",  {SCol::Right & SCol::Level2, -1 & SCol::Level2}},
  {"TarsusR2",      {SCol::Right & SCol::Level2, -1 & SCol::Level2}},
  {"FemurR3",       {SCol::Right & SCol::Level3, -1 & SCol::Center & SCol::Level3}},
  {"TrochanterR3",  {SCol::Right & SCol::Level3, -1 & SCol::Center & SCol::Level3}},
  {"PatellaR3",     {SCol::Right & SCol::Level3, -1 & SCol::Level3}},
  {"TibiaR3",       {SCol::Right & SCol::Level3, -1 & SCol::Level3}},
  {"MetatarsusR3",  {SCol::Right & SCol::Level3, -1 & SCol::Level3}},
  {"TarsusR3",      {SCol::Right & SCol::Level3, -1 & SCol::Level3}},
  {"TrochanterR4",  {SCol::Right & SCol::Level4, -1 & SCol::Center & SCol::Level4}},
  {"FemurR4",       {SCol::Right & SCol::Level4, -1 & SCol::Center & SCol::Level4}},
  {"PatellaR4",     {SCol::Right & SCol::Level4, -1 & SCol::Level4}},
  {"TibiaR4",       {SCol::Right & SCol::Level4, -1 & SCol::Level4}},
  {"MetatarsusR4",  {SCol::Right & SCol::Level4, -1 & SCol::Level4}},
  {"TarsusR4",      {SCol::Right & SCol::Level4, -1 & SCol::Level4}}
};

Spider::Spider(Asset* asset) : Logging::Log("Spider") {
  ResourceManager* r = asset->rManager();
  mProgram           = r->get<Program>("Program::Model");
  mMesh              = r->get<PhysicsMesh>("PhysicsMesh::Spider");
  mElements          = mMesh->createCopyAll();

  for (auto& mesh : mElements->meshes) {
    mLog->debug("Adding MeshPart: {}", mesh.first);
    mChildren.push_back(new MeshPart(mProgram,
                                     mesh.second,
                                     mElements->bodies[mesh.first],
                                     mElements->motions[mesh.first]));
    mChildren.back()->setCollisionGroup(SColFilters[mesh.first].first);
    mChildren.back()->setCollisionMask(SColFilters[mesh.first].second);

    for (auto& c : mElements->constraints[mesh.first]) {
      mChildren.back()->addConstraint(c);
    }
  }

  // for (auto subMesh : allSubmeshes) {
  //   mLog->debug("Adding MeshPart: {}", subMesh.first);
  //   mChildren.push_back(new MeshPart(mProgram, subMesh.second));
  // }
}

Spider::~Spider() {
  for (auto& c : mChildren)
    delete c;

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
  for (auto& child : mChildren)
    child->draw(c);
  mMesh->mesh()->unbindVertexArray();
}

void Spider::input(const Input::Event&) {}
