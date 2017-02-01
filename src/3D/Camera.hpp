#pragma once

#include <memory>
#include <mmm.hpp>
#include <string>

class Asset;
class Program;

namespace Input {
  class Event;
}

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

  Camera(Asset* a);

  mmm::mat4 updateViewMatrix() const;
  mmm::mat4 updateProjectionMatrix() const;

  void setLightVPUniforms(std::shared_ptr<Program> program,
                          const std::string&       name = "light");

  void setLightMVPUniform(std::shared_ptr<Program> program,
                          const std::string&       name = "lightMVP");

  void setLightMVPUniforms(std::shared_ptr<Program> program,
                           const std::string&       name = "light");

  void setMVPUniform(std::shared_ptr<Program> program,
                     const std::string&       name = "MVP");

  void setMVPUniforms(std::shared_ptr<Program> program);

  const Light& light() const;

  const mmm::mat4& model() const;
  const mmm::mat4& view() const;
  const mmm::mat4& projection() const;
  const mmm::vec3& target() const;

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

  float mPHoriRotation;
  float mPVertRotation;
};
