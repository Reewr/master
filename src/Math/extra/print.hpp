#ifndef MATH_PRINT_HPP
#define MATH_PRINT_HPP

std::ostream& operator<< (std::ostream& os, const vec2& v);
std::ostream& operator<< (std::ostream& os, const vec3& v);
std::ostream& operator<< (std::ostream& os, const vec4& v);
std::ostream& operator<< (std::ostream& os, const mat3& m);
std::ostream& operator<< (std::ostream& os, const mat4& m);

void print (const vec2& v);
void print (const vec3& v);
void print (const vec4& v);
void print (const mat3& m);
void print (const mat4& m);

#endif