#include "Camera.hpp"

#include "../GLSL/Program.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

Asset* Camera::asset = nullptr;
void Camera::init(Asset* asset) {
  Camera::asset = asset;
}

/* Camera::Camera (Input* i, Program* shadow, Program* model) { */
Camera::Camera(Input* i, Program* shadow) {
  this->shadow = shadow;
  /* this->model  = model; */
  light.day = 0;
  input     = i;

  mvp.proj = updateProjMatrix();

  /* model->setUniform("proj", mvp.proj); */

  update(0);
}

mat4 Camera::updateViewMatrix() {
  vec3 cp =
    target + vec3(rotate_y(hrot) * rotate_x(vrot) * vec4(0, 0, height, 1));
  vec3 cu = vec3(rotate_y(hrot) * rotate_x(vrot) * vec4(0, 1, 0, 0));
  return lookAt(cp, target, cu);
}

mat4 Camera::updateProjMatrix() {
  return perspective(67,
                     asset->cfg.graphics.aspect,
                     0.1,
                     asset->cfg.graphics.viewDistance);
}

void Camera::setLightMVPUniform(Program* p, std::string name) {
  p->setUniform(name, (mat4) light.mvp);
}

void Camera::setLightMVPUniforms(Program* p, std::string name) {
  p->setUniform(name + ".model", light.mvp.model);
  setLightMPUniforms(p, name);
}

void Camera::setLightMPUniforms(Program* p, std::string name) {
  p->setUniform(name + ".view", light.mvp.view);
  p->setUniform(name + ".proj", light.mvp.proj);
}
void Camera::setMVPUniform(Program* p, std::string name) {
  p->setUniform(name, (mat4) mvp);
}

void Camera::setMVPUniforms(Program* p, std::string name) {
  p->setUniform(name + ".model", mvp.model);
  p->setUniform(name + ".view", mvp.view);
  p->setUniform(name + ".proj", mvp.proj);
}

void Camera::update(float) {
  mvp.view = updateViewMatrix();

  // light.day -= light.speed * dt;
  mat4 lt = rotate_z(light.day) * rotate_y(hrot);
  vec3 lp = target + vec3(lt * vec4(0, height, 0, 1));
  vec3 lu = vec3(lt * vec4(0, 0, -1, 0));

  light.mvp.view = lookAt(lp, target, lu);
  // float h = height;
  // light.mvp.proj = ortho (-5*height, 5*height, -5*height, 5*height,
  // -5*height, 5*height);
  light.mvp.proj = ortho(-7, 7, -7, 7, -7, 7);
  light.dir      = normalize(lp - target);

  setLightMPUniforms(shadow);
  /* setLightMPUniforms(model); */
  /* model->setUniform ("view", mvp.view); */
  /* model->setUniform ("dir", light.dir); */
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
  sign = asset->cfg.camera.zoomInv ^ (sign > 0) ? -1 : 1;
  height += asset->cfg.camera.zoomSpeed * sign;
  if (height > 40)
    height = 40;
  if (height < 0.2)
    height = 0.2;
}

void Camera::handleKeys(std::vector<int> actions, float dt) {
  vec4 dir     = vec4(0, 0, -1, 0) * speed * dt;
  vec3 forward = vec3(rotate_y(hrot) * dir);
  vec3 strafe  = vec3(rotate_y(hrot + 90) * dir);

  for (unsigned int i = 0; i < actions.size(); i++) {
    if (actions[i] == Input::MOVE_UP)
      target += forward;
    if (actions[i] == Input::MOVE_DOWN)
      target -= forward;

    if (actions[i] == Input::MOVE_LEFT)
      target += strafe;
    if (actions[i] == Input::MOVE_RIGHT)
      target -= strafe;

    if (actions[i] == Input::ROTATE) {
      vec2 press = input->getPressedCoord(input->getKey(Input::ROTATE).key1);
      vec2 curr  = input->getMouseCoords();

      float rsh =
        asset->cfg.camera.rotSpeed * (asset->cfg.camera.rotInvH ? -1 : 1);
      float rsv =
        asset->cfg.camera.rotSpeed * (asset->cfg.camera.rotInvV ? -1 : 1);
      hrot = p_hrot + (curr - press).x / asset->cfg.graphics.res.x * rsh;
      vrot = p_vrot + (curr - press).y / asset->cfg.graphics.res.y * rsv;
    }
  }
}