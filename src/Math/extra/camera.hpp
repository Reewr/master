#ifndef MATH_CAMERA_HPP
#define MATH_CAMERA_HPP

//! Generates the matrix needed to look at one specific location
mat4 lookAt(const vec3& cameraPosition,
            const vec3& targetPosition,
            const vec3& up); // my horse!

//! Creates a perspective matrix
mat4 perspective(float fovy, float aspect, float near, float far);

//! Creates a ortographic matrix
mat4 ortho(float left,
           float right,
           float bottom,
           float top,
           float near,
           float far);

//! Unprojects the view
vec3 unProject(const vec3& win,
               const mat4& view,
               const mat4& proj,
               const vec4& viewport);

#endif