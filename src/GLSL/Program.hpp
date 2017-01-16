#pragma once

#include <map>
#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include "../Math/Math.hpp"
#include "../Utils/Utils.hpp"

class Shader;
class fstream;

//! Contains GLSL-Program and lets you load shaders,
//! bind and unbinds them easily.
//! NOTE: createProgram and link will cause segmentation fault
//! if it fails to create/link GLSL program.
class Program {
public:
  //! Default Constructor
  Program();

  Program(const Shader& frag, const Shader& vertex, const bool link = true);
  Program(const std::string& fs, const Shader& vertex, const bool link = true);
  Program(const Shader& frag, const std::string& fs, const bool link = true);
  Program(const std::string& fs, const std::string& vs, const bool link = true);

  // Should only be used on files containing more than one shader
  // Seperated by "#ifdef __VERTEX__" and ended with "#endif"
  // Seperated by "#ifdef __FRAGMENT__" and ended with "#endif"
  Program(const std::string& fsvs, int link = 1);

  //! Deletes shader program
  ~Program();

  //! Creates and links the program. If you specify link as false it will
  //! not link so you can set attrib values before hand.
  bool createProgram(const std::string& fs,
                     const std::string& vs,
                     const bool         link = true);
  bool createProgram(const std::string& fs,
                     const Shader&      vertex,
                     const bool         link = true);
  bool createProgram(const Shader&      frag,
                     const std::string& vs,
                     const bool         link = true);
  bool createProgram(const Shader& frag,
                     const Shader& vertex,
                     const bool    link = true);

  bool createProgram(const std::string& fsvs, int link = 1);

  void deleteProgram();

  //! Adds a shader to the current program
  bool addShader(const Shader& sh);
  bool addShader(const std::string& sh);

  //! Links the program if you specified link in createProgram() as false.
  //! Does nothing otherwise. Returns false if errors.
  bool link();

  //! Makes this program the active one. Returns true if already bound
  void bind();

  //! Returns the location of a uniform variable within the program.
  //! Note: Only works if createProgram() has been called
  GLint getUniformLocation(const std::string& uni);

  //! Returns the location of an attrib variable within the program.
  //! Note: Only works if createProgram() has been called
  GLint getAttribLocation(const std::string& atrib);

  //! Sets the variable in the shader program with name uni to value i.
  //! Returns false if no variable named uni is found
  template <typename T>
  bool setUniform(const std::string& uni, const T& t);

  //! Sets several uniforms with the names in unis to values is.
  //! Returns false if one fails. Lists has to be equal in size.
  template <typename T>
  bool setUniforms(const std::vector<std::string>& unis,
                   const std::vector<T>&           t);

  //! Set several uniform pairs ("uniformname", vec2, "uniformname2", int)
  template <typename T, typename... Ts>
  bool setUniform(const std::string& uni, T x, Ts... xs);

  //! Lets you bind an attribute (by name) to a index.
  //!  Only allowed if the program is not linked.
  bool bindAttrib(const std::string& attrib, const int index);
  bool bindAttribs(const std::vector<std::string>& attribs,
                   const std::vector<int>&         indexes);

  bool isActive() const;

  static bool checkErrors(const std::string&              place,
                          const std::vector<std::string>& files = {});

private:
  bool setGLUniform(GLint loc, const int i);
  bool setGLUniform(GLint loc, const int i, const int j);
  bool setGLUniform(GLint loc, const float f);
  bool setGLUniform(GLint loc, const double d);
  bool setGLUniform(GLint loc, const vec2& v);
  bool setGLUniform(GLint loc, const vec3& v);
  bool setGLUniform(GLint loc, const vec4& v);
  bool setGLUniform(GLint loc, const mat3& m);
  bool setGLUniform(GLint loc, const mat4& m);

  static bool checkProgram(const GLuint pro);
  static std::string loadShader(std::ifstream& f);
  static std::map<std::string, std::string> loadVSFS(const std::string& vsfs);

  static GLuint activeProgram;

  std::map<std::string, int> uniLocations;
  std::vector<std::string> filenames;

  GLuint program;

  bool isLinked;
  bool isUsable;
};

#include <GLSL/Program.tpp>