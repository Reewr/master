#include "Camera.hpp"

#include "../GLSL/Program.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"

using mmm::vec3;
using mmm::vec4;
using mmm::mat4;

Camera::Light::Light()
    : day(0)
    , speed(0.5)
    , ambient(0.2)
    , color(1, 1, 1)
    , model(mat4::identity)
    , view(mat4::identity)
    , projection(mat4::identity) {}

Camera::Camera(Asset* asset)
    : mAsset(asset)
    , mTarget(0, 1.183, 0)
    , mModel(mat4::identity)
    , mView(mat4::identity)
    , mProjection(mat4::identity)
    , mHeight(2)
    , mHoriRotation(0)
    , mVertRotation(-45) {
    /* , mSpeed(4) { */

  mProjection    = updateProjectionMatrix();
  mShadowProgram = mAsset->rManager()->get<Program>("Program::shadow");
  mModelProgram  = mAsset->rManager()->get<Program>("Program::model");
  mModelProgram->setUniform("projection", mProjection);
  update(0);
}

mat4 Camera::updateViewMatrix() {
  vec3 cameraEye =
    mTarget + vec3(mmm::rotate_y(mHoriRotation) * mmm::rotate_x(mVertRotation) *
                   vec4(0, 0, mHeight, 1));
  vec3 cameraUp = vec3(mmm::rotate_y(mHoriRotation) *
                       mmm::rotate_x(mVertRotation) * vec4(0, 1, 0, 0));

  return mmm::lookAt(cameraEye, mTarget, cameraUp);
}

mat4 Camera::updateProjectionMatrix() {
  return mmm::perspective<float>(67.0f,
                                 mAsset->cfg()->graphics.aspect,
                                 0.1f,
                                 mAsset->cfg()->graphics.viewDistance);
}

void Camera::setLightMVPUniform(std::shared_ptr<Program> program,
                                const std::string&       name) {
  program->setUniform(name, mLight.projection * mLight.view * mLight.model);
}

void Camera::setLightMVPUniforms(std::shared_ptr<Program> program,
                                 const std::string&       name) {
  program->setUniform(name + ".model", mLight.model);
  setLightMPUniforms(program, name);
}

void Camera::setLightMPUniforms(std::shared_ptr<Program> program,
                                const std::string&       name) {
  program->setUniform(name + ".view", mLight.view);
  program->setUniform(name + ".proj", mLight.projection);
}

void Camera::setMVPUniform(std::shared_ptr<Program> program,
                           const std::string&       name) {
  program->setUniform(name, mProjection * mView * mModel);
}

void Camera::setMVPUniforms(std::shared_ptr<Program> program,
                            const std::string&       name) {
  program->setUniform(name + ".model", mModel);
  program->setUniform(name + ".view", mView);
  program->setUniform(name + ".proj", mProjection);
}

void Camera::update(float) {
  mView = updateViewMatrix();

  // light.day -= light.speed * dt;
  mat4 lt       = mmm::rotate_z(mLight.day) * mmm::rotate_y(mHoriRotation);
  vec3 lightEye = mTarget + vec3(lt * vec4(0, mHeight, 0, 1));
  vec3 lightUp  = vec3(lt * vec4(0, 0, -1, 0));

  mLight.view = mmm::lookAt(lightEye, mTarget, lightUp);
  // float h = height;
  // light.proj = ortho (-5*height, 5*height, -5*height, 5*height,
  // -5*height, 5*height);
  mLight.projection = mmm::ortho(-7.f, 7.f, -7.f, 7.f, -7.f, 7.f);
  mLight.direction  = mmm::normalize(lightUp - mTarget);

  setLightMPUniforms(mShadowProgram);
  setLightMPUniforms(mModelProgram);
  mModelProgram->setUniform("view", mView);
  mModelProgram->setUniform("dir", mLight.direction);
  // terrain->setUniform("lightDir", ld);

  /*
  mat4 inv_view = transpose(view);
  mat3 inv_rot = {
    inv_view[0], inv_view[1], inv_view[2],
    inv_view[4], inv_view[5], inv_view[6],
    inv_view[8], inv_view[9], inv_view[10],
  };
  water->setUniform("inverse_rot", inv_rot);
  */
  // terrain->setUniform ("sunLight.color", light.color);
  // terrain->setUniform ("sunLight.direction", -ld);
  // terrain->setUniform ("sunLight.ambient", light.ambient);
}

void Camera::zoom(int sign) {
  CFG* cfg = mAsset->cfg();

  sign = cfg->camera.zoomInv ^ (sign > 0) ? -1 : 1;
  mHeight += cfg->camera.zoomSpeed * sign;

  if (mHeight > 40)
    mHeight = 40;

  if (mHeight < 0.2)
    mHeight = 0.2;
}

/* void Camera::handleKeys(const Input::Event& event, float dt) { */
/*   vec4 dir     = vec4(0, 0, -1, 0) * speed * dt; */
/*   vec3 forward = vec3(rotate_y(hrot) * dir); */
/*   vec3 strafe  = vec3(rotate_y(hrot + 90) * dir); */
/*   CFG* cfg     = asset->cfg(); */
/*   Input::Input* input = asset->input(); */

/*   if (event.isAction(Input::Action::MoveUp)) */
/*     target += forward; */
/*   if (event.isAction(Input::Action::MoveDown)) */
/*     target -= forward; */

/*   if (event.isAction(Input::Action::MoveLeft)) */
/*     target += strafe; */
/*   if (event.isAction(Input::Action::MoveRight)) */
/*     target -= strafe; */

/*   if (event.isAction(Input::Action::Rotate)) { */
/*     vec2 press = */
/*       event->getPressedCoord(input->getKey(Input::Action::Rotate).key1); */
/*     vec2 curr = input->getMouseCoords(); */

/*     float rsh = */
/*       cfg->camera.rotSpeed * (cfg->camera.rotInvH ? -1 : 1); */
/*     float rsv = */
/*       cfg->camera.rotSpeed * (cfg->camera.rotInvV ? -1 : 1); */
/*     hrot = p_hrot + (curr - press).x / cfg->graphics.res.x * rsh; */
/*     vrot = p_vrot + (curr - press).y / cfg->graphics.res.y * rsv; */
/*   } */
/* } */
