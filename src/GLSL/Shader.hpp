#pragma once

#include <string>

#include "../Log.hpp"
#include "../OpenGLHeaders.hpp"

class CFG;

//! Contains and loads the GLSL-Shaders
class Shader : public Logging::Log {
public:
  //! Default Constructor
  Shader();

  //! Constructor with path to file, calls loadShader()
  Shader(const std::string& filename);

  //! Deconstructor that deletes the GLSL-Shader if in memory
  ~Shader();

  //! Loads shader from file.
  //! Chooses shaderType based on file extension(.fs || ! .vs);
  GLuint loadShader(const std::string& filename);

  std::string filename() const;
  std::string type() const;
  GLuint      id() const;

  static CFG* mCFG;

private:
  //! Checks a shader for errors. If errors occure, calls handleShaderErrors
  bool checkShader();

  //! Shader ID. Used by OpenGL to get the shader from memory.
  GLuint      mId;
  std::string mFilename;
  std::string mType;
};
