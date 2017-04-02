#pragma once

#include <memory>
#include <mmm.hpp>
#include <string>

#include "../Log.hpp"

class Asset;
class Program;

namespace Input {
  class Event;
}

class Camera : public Logging::Log {
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

  // sets the model, view, projection matrices as separate
  // variables called model, view and proj
  void setMVPUniforms(std::shared_ptr<Program> program);

  // returns the light instance
  const Light& light() const;

  // Returns the position of the camera in the world
  const mmm::vec3& position() const;

  // Returns the model matrix
  const mmm::mat4& model() const;

  // returns the view matrix
  const mmm::mat4& view() const;

  // returns the projection matrix
  const mmm::mat4& projection() const;

  // returns the target vector
  const mmm::vec3& target() const;

  // Updates the camera to set a new view matrix
  void update(float dt);

  // adjust the camera by zooming in or out
  void zoom(int sign);

  // Handles the input. Should be used in update loop instead
  // of event based so it feels more fluid.
  void input(float dt);

  // Returns the end position of a ray from camera position to far plane.
  mmm::vec3 screenPointToRay(const mmm::vec2& mousePosition);

  // Turns a 3D position into a 2D screen coordinate
  mmm::vec3 project(const mmm::vec3& position);

private:
  Asset* mAsset;

  std::shared_ptr<Program> mShadowProgram;
  std::shared_ptr<Program> mModelProgram;

  mmm::vec3 mPosition;
  mmm::vec3 mTarget;
  mmm::vec3 mUp;
  mmm::mat4 mModel;
  mmm::mat4 mView;
  mmm::mat4 mProjection;

  float mSpeed;
  float mMinViewDistance;
  float mFieldOfView;
  Light mLight;
};
