#include "ExperimentUtil.hpp"

#include <btBulletDynamicsCommon.h>

/**
 * @brief
 *   Calculates a score based on on the `zeroIsBest` value.
 *   deltaTime is multiplied with it to make sure that it doesnt
 *   affect the rest of the simulations too much.
 *
 * @param deltaTime
 * @param zeroIsBest
 * @param bias
 *
 * @return
 */
float ExpUtil::score(float deltaTime, float zeroIsBest, float bias) {
  return 1.f / (mmm::max(mmm::abs(zeroIsBest) - bias, 0.f) * deltaTime + 1.f);
};


/**
 * @brief
 *   Returns the Euler angles of a Quaternion, as in the rotation
 *   around a specific axis.
 *
 * @param x
 * @param y
 * @param z
 * @param w
 *
 * @return
 */
mmm::vec3 ExpUtil::getEulerAngles(float x, float y, float z, float w) {
  double    sqw  = w * w;
  double    sqx  = x * x;
  double    sqy  = y * y;
  double    sqz  = z * z;
  double    unit = sqx + sqy + sqz + sqw;
  double    test = x * y + z * w;
  mmm::vec3 r;

  if (test > 0.499 * unit) { // singularity at north pole
    r.z = 2.0 * atan2(x, w);
    r.x = mmm::constants<double>::pi / 2.0;
    r.y = 0.0;
    return r;
  }
  if (test < -0.499 * unit) { // singularity at south pole
    r.z = -2.0 * atan2(x, w);
    r.x = -mmm::constants<double>::pi / 2.0;
    r.y = 0.0;
    return r;
  }
  r.z = atan2(2.0 * y * w - 2.0 * x * z, sqx - sqy - sqz + sqw);
  r.x = asin(2.0 * test / unit);
  r.y = atan2(2.0 * x * w - 2.0 * y * z, -sqx + sqy - sqz + sqw);

  return r;
}

/**
 * @brief
 *   Returns the Euler angles of a Quaternion, as in the rotation
 *   around a specific axis.
 *
 * @param q
 *
 * @return
 */
mmm::vec3 ExpUtil::getEulerAngles(const btQuaternion& q) {
  return getEulerAngles(q.x(), q.y(), q.z(), q.w());
}

/**
 * @brief
 *   Turns a btVector3 to an mmm::vec3
 *
 * @param v
 *
 * @return
 */
mmm::vec3 ExpUtil::fromVector(const btVector3& v) {
  return mmm::vec3(v.x(), v.y(), v.z());
}


/**
 * @brief
 *   Normalizes the hinge angle between -1 and 1 depending
 *   on what `low` and `up`.
 *
 * @param angle
 * @param low
 * @param up
 * @param rest
 *
 * @return
 */
float ExpUtil::normalizeAngle(float angle, float low, float up, float rest) {
  if (angle < low)
    angle += 2.f * mmm::constants<float>::pi;
  if (angle > up)
    angle -= 2.f * mmm::constants<float>::pi;

  if (angle - rest == 0.f)
    return 0.f;

  return angle < rest ? -(angle - rest) / (low - rest) :
                        (angle - rest) / (up - rest);
};

/**
 * @brief
 *   Does the opposite of normalizeHingeAngle given the same
 *   `low`, `up` and `rest` arguments.
 *
 * @param p
 * @param low
 * @param up
 * @param rest
 *
 * @return
 */
float ExpUtil::denormalizeAngle(float p, float low, float up, float rest) {
  return p < 0 ? p * mmm::abs(low - rest) + rest :
                 p * mmm::abs(up - rest) + rest;
};
