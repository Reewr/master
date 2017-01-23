#pragma once

#include <string>
#include <memory>
#include <mmm.hpp>

class Asset;
class Program;

class Camera {
public:
  struct Light {
    Light();

    float day;
    float speed;
    float ambient;

    mmm::vec3 color;
    mmm::vec3 direction;
    mmm::mat4 model;
    mmm::mat4 view;
    mmm::mat4 projection;
  };

  /* Program* model; */

  Camera(Asset* a);
  /* Camera (Input* i, Program* shadow, Program* model); */

  mmm::mat4 updateViewMatrix();
  mmm::mat4 updateProjectionMatrix();

  void setLightMPUniforms(std::shared_ptr<Program> program,
                          const std::string&       name = "light");

  void setLightMVPUniform(std::shared_ptr<Program> program,
                          const std::string&       name = "lightMVP");

  void setLightMVPUniforms(std::shared_ptr<Program> program,
                           const std::string&       name = "light");

  void setMVPUniform(std::shared_ptr<Program> program,
                     const std::string&       name = "MVP");

  void setMVPUniforms(std::shared_ptr<Program> program,
                      const std::string&       name = "mvp");

  void update(float dt);
  void zoom(int sign);

private:
  Asset* mAsset;

  std::shared_ptr<Program> mShadowProgram;
  std::shared_ptr<Program> mModelProgram;

  mmm::vec3 mTarget;
  mmm::mat4 mModel;
  mmm::mat4 mView;
  mmm::mat4 mProjection;

  float mHeight;
  float mHoriRotation;
  float mVertRotation;
  float mSpeed;
  Light mLight;

  // What are these two?
  /* float p_hrot = 0; */
  /* float p_vrot = 0; */
};
