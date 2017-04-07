#include "Line.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Line.hpp"
#include "../Utils/Asset.hpp"

using mmm::vec2;
using mmm::vec3;

Line::Line(const mmm::vec3& start, const mmm::vec3& end, const mmm::vec4& color)
    : Logging::Log("Line") , mLine(new GLLine()){
  mTexture = mAsset->rManager()->get<Texture>("Texture::Line");
  mColor = color;

  mUsesColor = color.x != -1;

  vec3 diffS = vec3(0,0,0) - start;
  vec3 diffE = vec3(1,0,0) - end;
  // find needed rotation and scale to match start and end point
  vec3 scale = diffE - diffS;
  if (scale.x == 0) {
    scale.x = 1;
  }
  if (scale.y == 0) {
    scale.y = 1;
  }

  if (scale.z == 0) {
    scale.z = 1;
  }

  mLog->debug("Test: {}", scale);
  mPosition = vec3(start + end) / 2.0;
  mScale    = mmm::scale(mmm::abs(scale));
}

Line::~Line() {}

/**
 * @brief
 *   The update handler is reversed for things that do not have
 *   anything to do with drawing but should be updated every loop.
 *
 * @param float
 */
void Line::update(float) {}

/**
 * @brief
 *   Draw the cube by binding the program and setting the required
 *   uniforms.
 *
 * @param c
 */
void Line::draw(std::shared_ptr<Program>& program, bool bindTexture) {
  draw(program, mmm::vec3(0), bindTexture);
}

/**
 * @brief
 *   Draw the cube by binding the program and setting the required
 *   uniforms.
 *
 * @param c
 */
void Line::draw(std::shared_ptr<Program>& program,
                mmm::vec3                 offset,
                bool                      bindTexture) {
  program->bind();
  program->setUniform("model",
                      mmm::translate(mPosition + offset) * mRotation * mScale);

  if (mUsesColor)
    program->setUniform("overrideColor", mColor);
  else
    program->setUniform("overrideColor", mmm::vec4(-1));

  if (bindTexture)
    mTexture->bind(1);
  mLine->draw();
}

void Line::input(const Input::Event&) {}
