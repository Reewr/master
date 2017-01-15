#ifndef THREED_CAMERA_HPP
#define THREED_CAMERA_HPP

#include <string>
#include <vector>

#include "../Math/Math.hpp"

class Asset;
class Program;
class Input;

struct Camera {
  vec3  target = { 0, 1.183, 0 };
  float height = 2;

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
    MVP   mvp;
  } light;

  Program* shadow;
  /* Program* model; */

  Input* input;

  MVP mvp;

  Camera(Input* i, Program* shadow);
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

  void handleKeys(std::vector<int> actions, float dt);

  static Asset* asset;
  static void init(Asset* asset);
};

#endif
