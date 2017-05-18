#include "Camera.hpp"

#include "../GLSL/Program.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"

using mmm::vec2;
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
    : Logging::Log("Camera")
    , mAsset(asset)
    , mPosition(6, 4, 6)
    , mTarget(2, 0, 2)
    , mUp(0, 1, 0)
    , mModel(mat4::identity)
    , mView(mat4::identity)
    , mProjection(mat4::identity)
    , mSpeed(4)
    , mMinViewDistance(1.f)
    , mFieldOfView(67.0f) {

  mView       = updateViewMatrix();
  mProjection = updateProjectionMatrix();

  float d           = mAsset->cfg()->graphics.viewDistance / 2.f;
  mLight.projection = mmm::ortho_r(-d, d, -d, d, -d, d);

  mShadowProgram = mAsset->rManager()->get<Program>("Program::Shadow");
  mModelProgram  = mAsset->rManager()->get<Program>("Program::Model");
  update(0);

  setMVPUniforms(mModelProgram);
}

/**
 * @brief
 *   Retrieves a view matrix based on current camera position and view angle.
 *
 * @return
 */
mat4 Camera::updateViewMatrix() const {
  return mmm::lookAt_r(mPosition, mTarget, mUp);
}

/**
 * @brief
 *   Retrieves a projection matrix based on the aspect ratio and view distance
 *   defined in the configuration file.
 *
 * @return
 */
mat4 Camera::updateProjectionMatrix() const {
  return mmm::perspective_r<float>(mFieldOfView,
                                   mAsset->cfg()->graphics.aspect,
                                   mMinViewDistance,
                                   mAsset->cfg()->graphics.viewDistance);
}

/**
 * @brief
 *   Sets a uniform of `name` in `program` to the MVP value calculated
 *   by multiplying projection, view and model together
 *
 *   This uses the Light's MVP.
 *
 * @param program the program to set the variable in
 * @param name the name of the uniform variable.
 */
void Camera::setLightMVPUniform(std::shared_ptr<Program> program,
                                const std::string&       name) {
  program->setUniform(name, mLight.projection * mLight.view * mLight.model);
}

/**
 * @brief
 *   Sets three uniforms that is expected to be within a struct. The struct
 *   is expected to have three members, `model`, `view` and `proj`, all
 *   of which are mat4 types.
 *
 *   The `name` indicates the name of an instance of the structure.
 *
 * @param program
 * @param name
 */
void Camera::setLightMVPUniforms(std::shared_ptr<Program> program,
                                 const std::string&       name) {
  program->setUniform(name + ".model", mLight.model);
  setLightVPUniforms(program, name);
}

/**
 * @brief
 *   Does the same as the function above, but does not set
 *   the `model` type.
 *
 * @param program
 * @param name
 */
void Camera::setLightVPUniforms(std::shared_ptr<Program> program,
                                const std::string&       name) {
  program->setUniform(name + ".view", mLight.view);
  program->setUniform(name + ".proj", mLight.projection);
}

/**
 * @brief
 *   Sets a uniform of `name` in `program` to the MVP value calculated
 *   by multiplying projection, view and model together
 *
 *   This uses the model, view and projection stored on the camera
 *
 * @param program the program to set the variable in
 * @param name the name of the uniform variable.
 */
void Camera::setMVPUniform(std::shared_ptr<Program> program,
                           const std::string&       name) {
  program->setUniform(name, mProjection * mView * mModel);
}

/**
 * @brief
 *   This function expects to find three different uniform mat4
 *   variables in the given program with name `model`, `view`
 *   and `proj` and sets them to be the model matrix, view matrix
 *   and projection matrix, respectively.
 *
 * @param program
 */
void Camera::setMVPUniforms(std::shared_ptr<Program> program) {
  program->setUniform("model", mModel);
  program->setUniform("view", mView);
  program->setUniform("proj", mProjection);
}

/**
 * @brief
 *   Returns a const reference to the Light instance that
 *   the camera has.
 *
 * @return
 */
const Camera::Light& Camera::light() const {
  return mLight;
}

/**
 * @brief
 *   Returns a const reference to the model matrix
 *
 * @return
 */
const mat4& Camera::model() const {
  return mModel;
}

/**
 * @brief
 *   Returns a const reference to the view matrix
 *
 * @return
 */
const mat4& Camera::view() const {
  return mView;
}

/**
 * @brief
 *   Returns a const reference to the projection matrix
 *
 * @return
 */
const mat4& Camera::projection() const {
  return mProjection;
}

/**
 * @brief
 *   Returns a const reference to the target vector
 *
 * @return
 */
const vec3& Camera::target() const {
  return mTarget;
}

/**
 * @brief
 *   Returns a const reference to the position vector
 *
 * @return
 */
const vec3& Camera::position() const {
  return mPosition;
}

/**
 * @brief
 *   This function should be called on every iteration, prior to calling
 *   update(float). This handles the input.
 *
 * @param dt
 */
void Camera::input(float dt) {
  static vec2 previousMouse;

  vec3  forward = mmm::normalize(mTarget - mPosition) * mSpeed * dt;
  vec3  left    = vec3(mmm::rotate_y(90.f) * vec4(forward.x, 0, forward.z, 1));
  CFG*  cfg     = mAsset->cfg();
  auto* input   = mAsset->input();

  std::vector<int> actions = input->getPressedActions();

  // handle all the inputs that have been pressed
  for (auto a : actions) {

    if (a == Input::Action::MoveUp) {
      mTarget += forward;
      mPosition += forward;
    } else if (a == Input::Action::MoveDown) {
      mTarget -= forward;
      mPosition -= forward;
    } else if (a == Input::Action::MoveRight) {
      mTarget -= left;
      mPosition -= left;
    } else if (a == Input::Action::MoveLeft) {
      mTarget += left;
      mPosition += left;
    } else if (a == Input::Action::Rotate) {

      if (previousMouse.x == 0 && previousMouse.y == 0) {
        int key       = input->getKey(Input::Action::Rotate).key1;
        previousMouse = input->getPressedCoord(key);
      }

      vec2 mouse = input->getMouseCoords();
      vec2 diff  = mouse - previousMouse;

      vec2 rs = vec2(cfg->camera.rotSpeed);
      rs.x *= cfg->camera.rotInvH ? -1 : 1;
      rs.y *= cfg->camera.rotInvV ? -1 : 1;
      vec2 rot = diff / cfg->graphics.res * rs;

      forward = vec3(mmm::rotate(-rot.y, left) * vec4(forward, 1));
      mTarget = mPosition + vec3(mmm::rotate(rot.x, mUp) * vec4(forward, 1));

      previousMouse = mouse;
    }
  }

  if (!input->isActionPressed(Input::Action::Rotate)) {
    previousMouse = vec2();
  }
}

/**
 * @brief
 *   This function should be updated on every iteration as it
 *   handles changes to the camera values.
 *
 * @param dt
 */
void Camera::update(float) {

  // now that input has been handled, handle the new positions and stuff
  // that may have been set
  mView = updateViewMatrix();

  // mLight.day -= mLight.speed * dt;

  mat4  lt = mmm::rotate_z(mLight.day);
  float d  = mAsset->cfg()->graphics.viewDistance / 2.f;

  vec3 up     = vec3(0, 0, -1);
  vec3 target = mPosition + mmm::normalize(mTarget - mPosition) * d;
  vec3 eye    = target + vec3(lt * vec4(mUp, 1));

  mLight.view = mmm::lookAt_r(eye, target, up);
  // mLight.direction = -mmm::normalize(up - mTarget);
  mLight.direction = vec3(lt * vec4(0, 1, 0, 1));

  // terrain->setUniform ("sunLight.color", light.color);
  // terrain->setUniform ("sunLight.direction", -ld);
  // terrain->setUniform ("sunLight.ambient", light.ambient);
}

/**
 * @brief
 *   When a user uses the scroll wheel, zoom in and out
 *   with the camera.
 *
 * @param sign
 */
void Camera::zoom(int sign) {
  CFG*  cfg = mAsset->cfg();
  float s = cfg->camera.zoomSpeed * (cfg->camera.zoomInv ^ (sign > 0) ? -1 : 1);
  mFieldOfView = mmm::clamp(mFieldOfView + s, 32.f, 128.f);

  mLog->debug("fov: {}", mFieldOfView);
  mProjection = updateProjectionMatrix();
}

/**
 * @brief
 *   As the mouse pointer has a 2d coordinate, it has to projected
 *   as a ray down into the 3D world.
 *
 *   This function effectively simulates that ray and checks if
 *   the ray hits any physics objects.
 *
 *   Returns the position of where the mouse coordinate is projected
 *   down to.
 *
 * @param mousePosition
 *
 * @return
 */
vec3 Camera::screenPointToRay(const vec2& mp) {
  vec4 viewport = vec4(0, 0, mAsset->cfg()->graphics.res);
  vec3 win      = vec3(mp.x, viewport.w - mp.y, 1.f);
  vec3 d        = unProject(win, mView, mProjection, viewport);
  return d;
}

/**
 * @brief
 *   Takes a 3D position and returns the equivalent 2D position
 *   on the screen
 *
 * @param position
 *
 * @return
 */
vec3 Camera::project(const vec3& position) {
  vec4 tmp      = vec4(position, 1.0);
  vec4 viewport = vec4(0, 0, mAsset->cfg()->graphics.res);
  tmp           = mModel * tmp;
  tmp           = mProjection * tmp;

  tmp /= tmp.w;
  tmp = tmp * 0.5f + 0.5f;

  tmp.x = tmp.x * viewport.z + viewport.z;
  tmp.y = tmp.y * viewport.w + viewport.y;

  return tmp.xyz;
}
