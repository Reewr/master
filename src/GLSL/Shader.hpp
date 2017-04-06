#pragma once

#include <string>

#include "../Log.hpp"
#include "../OpenGLHeaders.hpp"

class CFG;

//! Contains and loads the GLSL-Shaders
class Shader : public Logging::Log {
public:
  struct LayoutBinding {
    int         location;
    std::string name;
  };

  struct Details {
    std::vector<LayoutBinding> layoutBindings;
    std::vector<std::string>   includedFiles;
    std::string                source;
  };

  // This describes the type of shader
  enum class Type {
    Fragment,
    Vertex,
    Geometry,
    Tessellation,
    Evaluation,
    Compute,
    None
  };

  //! Default Constructor
  Shader();

  // Constructor with path to file, calls loadShader()
  // It will use the type given if it is not None. If it is None, it will
  // retrieve the type from the filename. Valid file-extensions are:
  //
  // - ".fs" for Fragment Shader
  // - ".vs" for Vertex Shader
  // - ".gs" for Geometry Shader
  // - ".ts" for Tesselation Control Shader
  // - ".es" for Tesselation Evaluation Shader
  // - ".cs" for Compute Shader
  Shader(const std::string& filename, Type type = Type::None);

  //! Deconstructor that deletes the GLSL-Shader if in memory
  ~Shader();

  // Loads shader from file.
  // It will use the type given if it is not None. If it is None, it will
  // retrieve the type from the filename. Valid file-extensions are:
  //
  // - ".fs" for Fragment Shader
  // - ".vs" for Vertex Shader
  // - ".gs" for Geometry Shader
  // - ".ts" for Tesselation Control Shader
  // - ".es" for Tesselation Evaluation Shader
  // - ".cs" for Compute Shader
  GLuint loadShader(const std::string& filename, Type type = Type::None);

  const Details& details() const;
  std::string    filename() const;
  Type           type() const;
  GLuint         id() const;

  static CFG* mCFG;

  static Type typeFromFilename(const std::string& name);
  static GLuint typeToGLType(Type type);
  static std::string typeToStr(Type t);

private:
  //! Checks a shader for errors. If errors occure, calls handleShaderErrors
  bool checkShader();

  //! Shader ID. Used by OpenGL to get the shader from memory.
  GLuint      mId;
  std::string mFilename;
  Type        mType;

  Details mDetail;
};
