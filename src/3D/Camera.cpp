#include "Camera.hpp"

#include "../GLSL/Program.hpp"
#include "../Input/Input.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

/* Camera::Camera (Input* i, Program* shadow, Program* model) { */
Camera::Camera(Asset* a, Program* shadow) {
  this->shadow = shadow;
  asset        = a;
  /* this->model  = model; */
  light.day = 0;
  proj      = updateProjMatrix();

  /* model->setUniform("proj", proj); */

  update(0);
}

mat4 Camera::updateViewMatrix() {
  vec3 cp =
    target + vec3(rotate_y(hrot) * rotate_x(vrot) * vec4(0, 0, height, 1));
  vec3 cu = vec3(rotate_y(hrot) * rotate_x(vrot) * vec4(0, 1, 0, 0));
  return lookAt(cp, target, cu);
}

mat4 Camera::updateProjMatrix() {
  return perspective<float>(67.f,
                            asset->cfg()->graphics.aspect,
                            0.1f,
                            asset->cfg()->graphics.viewDistance);
}

void Camera::setLightMVPUniform(Program* p, std::string name) {
  p->setUniform(name, light.proj * light.view * light.model);
}

void Camera::setLightMVPUniforms(Program* p, std::string name) {
  p->setUniform(name + ".model", light.model);
  setLightMPUniforms(p, name);
}

void Camera::setLightMPUniforms(Program* p, std::string name) {
  p->setUniform(name + ".view", light.view);
  p->setUniform(name + ".proj", light.proj);
}
void Camera::setMVPUniform(Program* p, std::string name) {
  p->setUniform(name, proj * view * model);
}

void Camera::setMVPUniforms(Program* p, std::string name) {
  p->setUniform(name + ".model", model);
  p->setUniform(name + ".view", view);
  p->setUniform(name + ".proj", proj);
}

void Camera::update(float) {
  view = updateViewMatrix();

  // light.day -= light.speed * dt;
  mat4 lt = rotate_z(light.day) * rotate_y(hrot);
  vec3 lp = target + vec3(lt * vec4(0, height, 0, 1));
  vec3 lu = vec3(lt * vec4(0, 0, -1, 0));

  light.view = lookAt(lp, target, lu);
  // float h = height;
  // light.proj = ortho (-5*height, 5*height, -5*height, 5*height,
  // -5*height, 5*height);
  light.proj = ortho(-7.f, 7.f, -7.f, 7.f, -7.f, 7.f);
  light.dir  = normalize(lp - target);

  setLightMPUniforms(shadow);
  /* setLightMPUniforms(model); */
  /* model->setUniform ("view", view); */
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
  CFG* cfg = asset->cfg();

  sign = cfg->camera.zoomInv ^ (sign > 0) ? -1 : 1;
  height += cfg->camera.zoomSpeed * sign;
  if (height > 40)
    height = 40;
  if (height < 0.2)
    height = 0.2;
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
