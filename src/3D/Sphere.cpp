#include "Sphere.hpp"

#include <btBulletDynamicsCommon.h>

#include "../GLSL/Program.hpp"
#include "../Graphical/Framebuffer.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Sphere.hpp"
#include "../Utils/Asset.hpp"

using mmm::vec2;
using mmm::vec3;

Sphere::Sphere(const mmm::vec3& position,
               mmm::vec3 size,
               std::shared_ptr<Texture> texture)
    : Logging::Log("Sphere")
    , mSphere(new GLSphere())
    , mTexture(texture) {
      mScale = mmm::scale(size);
      mPosition = position;
      mRotation = mmm::rotate_x(-90.f);
    }

Sphere::~Sphere() {
  delete mSphere;
}

/**
 * @brief
 *   The update handler is reversed for things that do not have
 *   anything to do with drawing but should be updated every loop.
 *
 * @param float
 */
void Sphere::update(float) {}

/**
 * @brief
 *   Draw the sphere by binding the program and setting the required
 *   uniforms.
 *
 * @param c
 */
void Sphere::draw(std::shared_ptr<Program>& program, bool bindTexture) {
  draw(program, mmm::vec3(0), bindTexture);
}

/**
 * @brief
 *   Draw the sphere by binding the program and setting the required
 *   uniforms.
 *
 * @param c
 */
void Sphere::draw(std::shared_ptr<Program>& program,
                mmm::vec3                 offset,
                bool                      bindTexture) {
  program->bind();
  program->setUniform("model",
                      mmm::translate(mPosition + offset) * mRotation * mScale);

  if (bindTexture && mTexture != nullptr)
    mTexture->bind(1);
  mSphere->draw();
}

void Sphere::input(const Input::Event&) {}
