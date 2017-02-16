#pragma once

#include <map>
#include <string>
#include <vector>

#include <mmm.hpp>

#include "../OpenGLHeaders.hpp"
#include "../Resource/Resource.hpp"

class Shader;
class fstream;
class ResourceManager;

//! Contains GLSL-Program and lets you load shaders,
//! bind and unbinds them easily.
//! NOTE: createProgram and link will cause segmentation fault
//! if it fails to create/link GLSL program.
class Program : public Resource {
public:
  //! Default Constructor
  Program();

  // Should only be used on files containing more than one shader
  // Seperated by "#ifdef __VERTEX__" and ended with "#endif"
  // Seperated by "#ifdef __FRAGMENT__" and ended with "#endif"
  //
  // This can also be used when the filename contains two files, separated
  // by comma, for instance: "path/to/myshader.fs,path/to/myshader.vs"
  Program(const std::string& fsvs, bool link = true);

  //! Deletes shader program
  ~Program();

  //! Creates and links the program. If you specify link as false it will
  //! not link so you can set attrib values before hand.
  bool createProgram(const std::string& fsvs, bool link = true);

  //! Adds a shader to the current program
  bool addShader(const Shader& sh);
  bool addShader(const std::string& sh);

  bool load(ResourceManager*);
  void unload();

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

  //! Lets you bind an attribute (by name) to a index.
  //!  Only allowed if the program is not linked.
  bool bindAttrib(const std::string& attrib, const int index);
  bool bindAttribs(const std::vector<std::string>& attribs,
                   const std::vector<int>&         indexes);

  bool isActive() const;

  static bool checkErrors(const std::string&              place,
                          const std::vector<std::string>& files = {});

private:
  bool setGLUniform(GLint loc, const bool b);
  bool setGLUniform(GLint loc, const int i);
  bool setGLUniform(GLint loc, const int i, const int j);
  bool setGLUniform(GLint loc, const float f);
  bool setGLUniform(GLint loc, const double d);
  bool setGLUniform(GLint loc, const mmm::vec2& v);
  bool setGLUniform(GLint loc, const mmm::vec3& v);
  bool setGLUniform(GLint loc, const mmm::vec4& v);
  bool setGLUniform(GLint loc, const mmm::mat3& m);
  bool setGLUniform(GLint loc, const mmm::mat4& m);

  static bool checkProgram(const GLuint pro);

  static std::map<std::string, std::string>
  loadDualShaderFilename(const std::string& vsfs);

  static GLuint activeProgram;

  std::map<std::string, int> uniLocations;
  std::vector<std::string> filenames;

  GLuint program = 0;

  bool isLinked = false;
  bool isUsable = false;
};

// ----------------------------------------------------------
//
// Below follows the implementation of the template functions
//
// ----------------------------------------------------------
template <typename T>
bool Program::setUniform(const std::string& uni, const T& t) {
  bind();
  GLint loc = getUniformLocation(uni);
  if (loc != -1)
    return setGLUniform(loc, t);
  return false;
}
