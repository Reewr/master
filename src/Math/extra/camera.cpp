#include "../Math.hpp"

mat4 lookAt(const vec3& cameraPosition,
            const vec3& targetPosition,
            const vec3& up) {
  vec3 f = normalize(targetPosition - cameraPosition);
  vec3 u = normalize(up);
  vec3 s = normalize(cross(f, u));
  u      = cross(s, f);

  return mat4(s.x,
              s.y,
              s.z,
              -dot(s, cameraPosition),
              u.x,
              u.y,
              u.z,
              -dot(u, cameraPosition),
              -f.x,
              -f.y,
              -f.z,
              dot(f, cameraPosition),
              0,
              0,
              0,
              1);
}

mat4 perspective(float fovy, float aspect, float near, float far) {
  float r  = tan(fovy * PI / 360.0) * near;
  float sx = (2.0 * near) / (r * aspect + r * aspect);
  float sy = near / r;
  float sz = -(far + near) / (far - near);
  float pz = -(2.0 * far * near) / (far - near);

  return mat4(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, pz, 0, 0, -1, 0);
}

mat4 ortho(float left,
           float right,
           float bottom,
           float top,
           float near,
           float far) {
  return mat4(2.0 / (right - left),
              0,
              0,
              -(right + left) / (right - left),
              0,
              2.0 / (top - bottom),
              0,
              -(top + bottom) / (top - bottom),
              0,
              0,
              -2.0 / (far - near),
              -(far + near) / (far - near),
              0,
              0,
              0,
              1);
}

vec3 unProject(const vec3& win,
               const mat4& view,
               const mat4& proj,
               const vec4& viewport) {
  vec4 d = vec4(win, 1);
  d.x    = (d.x - viewport.x) / viewport.z;
  d.y    = (d.y - viewport.y) / viewport.w;
  d      = inverse(proj * view) * (d * 2.0 - 1.0);

  return vec3(d / d.w);
}
