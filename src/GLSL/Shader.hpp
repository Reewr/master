#pragma once

#include <string>

#include "../OpenGLHeaders.hpp"

class CFG;

//! Contains and loads the GLSL-Shaders
class Shader {
public:
  //! Default Constructor
  Shader();

  //! Constructor with path to file, calls loadShader()
  Shader(const std::string& filename);

  Shader(const std::string& src,
         bool               isFragment,
         const std::string& fname = "from source");

  //! Deconstructor that deletes the GLSL-Shader if in memory
  ~Shader();

  GLuint
  fromSource(const std::string& src, bool isFragment, const std::string& fname);

  //! Loads shader from file. Chooses shaderType based on file extension(.fs ||
  //! .vs);
  GLuint loadShader(const std::string& filename);

  std::string filename() const;
  std::string type() const;
  GLuint      id() const;

  static CFG* mCFG;

private:
  //! Checks a shader for errors. If errors occure, calls handleShaderErrors
  static bool checkShader(const GLuint id, const std::string& filename);

  //! Shader ID. Used by OpenGL to get the shader from memory.
  GLuint      mId;
  std::string mFilename;
  std::string mType;
};
