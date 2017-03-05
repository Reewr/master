#include "MeshPart.hpp"

#include "../Resource/PhysicsMesh.hpp"
#include "../Resource/Mesh.hpp"

#include <btBulletDynamicsCommon.h>

MeshPart::MeshPart(const SubMeshPhysics&) : Logging::Log("MeshPart"), mMesh(nullptr) { }
MeshPart::~MeshPart() {}

void MeshPart::update(float) {}
void MeshPart::drawShadow(Framebuffer*, Camera*) {}
void MeshPart::draw(Camera*) {}
void MeshPart::input(const Input::Event&) {}
