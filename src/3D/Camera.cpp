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
    , mTarget(0, 1.183, 1)
    , mModel(mat4::identity)
    , mView(mat4::identity)
    , mProjection(mat4::identity)
    , mHeight(2)
    , mHoriRotation(0)
    , mVertRotation(45)
    , mSpeed(4)
    , mMinViewDistance(0.1f)
    , mFieldOfView(67.0f) {

  mProjection    = updateProjectionMatrix();
  mShadowProgram = mAsset->rManager()->get<Program>("Program::Shadow");
  mModelProgram  = mAsset->rManager()->get<Program>("Program::Model");
  update(0);

  setMVPUniforms(mModelProgram);
}

/**
 * @brief
 *   Updates the view matrix based on the vertical and horizontal rotation as
 *   well as the target.
 *
 * @return
 */
const mat4& Camera::updateViewMatrix() {
  vec3 cameraEye =
    mTarget + vec3(mmm::rotate_y(mHoriRotation) * mmm::rotate_x(mVertRotation) *
                   vec4(0, 0, mHeight, 1));
  vec3 cameraUp = vec3(mmm::rotate_y(mHoriRotation) *
                       mmm::rotate_x(mVertRotation) * vec4(0, 1, 0, 0));

  mPosition = cameraEye;
  mView = mmm::lookAt(cameraEye, mTarget, cameraUp);

  return mView;
}

/**
 * @brief
 *   Retrieves a projection matrix based on the aspect ratio and view distance
 *   defined in the configuration file.
 *
 * @return
 */
mat4 Camera::updateProjectionMatrix() const {
  return mmm::perspective<float>(mFieldOfView,
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
 *   This function should be called on every iteration, prior to calling
 *   update(float). This handles the input.
 *
 * @param dt
 */
void Camera::input(float dt) {
  // handle input first so that you get smooth movement
  vec4          dir     = vec4(0, 0, -1, 0) * mSpeed * dt;
  vec3          forward = vec3(mmm::rotate_y(mHoriRotation) * dir);
  vec3          strafe  = vec3(mmm::rotate_y(mHoriRotation + 90) * dir);
  CFG*          cfg     = mAsset->cfg();
  Input::Input* input   = mAsset->input();

  std::vector<int> actions = input->getPressedActions();

  // handle all the inputs that have been pressed
  for (auto a : actions) {
    if (a == Input::Action::MoveUp)
      mTarget += forward;
    else if (a == Input::Action::MoveDown)
      mTarget -= forward;
    else if (a == Input::Action::MoveRight)
      mTarget -= strafe;
    else if (a == Input::Action::MoveLeft)
      mTarget += strafe;
    else if (a == Input::Action::Rotate) {
      int  key   = input->getKey(Input::Action::Rotate).key1;
      vec2 press = input->getPressedCoord(key);
      vec2 diff  = input->getMouseCoords() - press;

      float rsh     = cfg->camera.rotSpeed * (cfg->camera.rotInvH ? -1 : 1);
      float rsv     = cfg->camera.rotSpeed * (cfg->camera.rotInvV ? -1 : 1);
      mHoriRotation = mHoriRotation + diff.x / cfg->graphics.res.x * rsh;
      mVertRotation = mVertRotation + diff.y / cfg->graphics.res.y * rsv;
    }
  }
}

/**
 * @brief
 *   This function should be updated on every iteration as it
 *   handles changes to the camera values.
 *
 * @param dt
 */
void Camera::update(float dt) {
  // now that input has been handled, handle the new positions and stuff
  // that may have been set
  updateViewMatrix();

  // mLight.day -= mLight.speed * dt;

  mat4 lt       = mmm::rotate_z(mLight.day) * mmm::rotate_y(mHoriRotation);
  vec3 lightEye = mTarget + vec3(lt * vec4(0, mHeight, 0, 1));
  vec3 lightUp  = vec3(lt * vec4(0, 0, -1, 0));

  mLight.view       = mmm::lookAt(lightEye, mTarget, lightUp);
  float h           = 5 * mHeight;
  mLight.projection = mmm::ortho(-h, h, -h, h, -h, h);
  // mLight.projection = mmm::ortho(-7.f, 7.f, -7.f, 7.f, -7.f, 7.f);
  mLight.direction = mmm::normalize(lightUp - mTarget);


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

/**
 * @brief
 *   When a user uses the scroll wheel, zoom in and out
 *   with the camera.
 *
 * @param sign
 */
void Camera::zoom(int sign) {
  CFG* cfg = mAsset->cfg();

  sign = cfg->camera.zoomInv ^ (sign > 0) ? -1 : 1;
  mHeight += cfg->camera.zoomSpeed * sign;

  if (mHeight > 60)
    mHeight = 60;

  if (mHeight < 0.1)
    mHeight = 0.1;
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
mmm::vec3 Camera::screenPointToRay(const mmm::vec2& mousePosition) {
  return unProject(
    vec3(mousePosition, 1.0),
    mView,
    mProjection,
    vec4(0, 0, mAsset->cfg()->graphics.res)
  );
}
