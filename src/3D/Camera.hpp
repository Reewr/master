#pragma once

#include <string>
#include <vector>

#include "../Math/Math.hpp"

class Asset;
class Program;

namespace Input {
class Input;
}

class Camera {
public:
  Asset* asset;
  vec3   target = { 0, 1.183, 0 };
  float  height = 2;

  float hrot   = 0;
  float vrot   = -45;
  float p_hrot = 0;
  float p_vrot = 0;

  float speed = 4;

  struct {
    float day     = 0;
    float speed   = 0.5;
    vec3  color   = { 1, 1, 1 };
    float ambient = 0.2;
    vec3  dir;
    mat4  model = mat4::identity;
    mat4  view  = mat4::identity;
    mat4  proj  = mat4::identity;
  } light;

  Program* shadow;
  /* Program* model; */

  mat4 model = mat4::identity;
  mat4 view  = mat4::identity;
  mat4 proj  = mat4::identity;

  Camera(Asset* a, Program* shadow);
  /* Camera (Input* i, Program* shadow, Program* model); */

  mat4 updateViewMatrix();
  mat4 updateProjMatrix();

  void setLightMVPUniform(Program* p, std::string name = "lightMVP");
  void setLightMPUniforms(Program* p, std::string name = "light");
  void setLightMVPUniforms(Program* p, std::string name = "light");
  void setMVPUniform(Program* p, std::string name = "MVP");
  void setMVPUniforms(Program* p, std::string name = "mvp");

  void update(float dt);
  void zoom(int sign);

  /* void handleKeys(std::vector<int> actions, float dt); */
};
