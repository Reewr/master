#pragma once

#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>
#include <mmm.hpp>

#include "../OpenGLHeaders.hpp"

class ResourceManager;
class Program;

class BulletDebugDraw : public btIDebugDraw {
  struct Vertex {
    mmm::vec3 point;
    mmm::vec3 color;
  };

  BulletDebugDraw(ResourceManager* manager);

  void
  drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

  void draw();

  void reset();

private:
  std::vector<Vertex> vertices;
  GLuint mVBO;
  GLuint mVAO;
  std::shared_ptr<Program> mLineProgram;
};
